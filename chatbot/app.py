"""
Chatbot Tư vấn Tuyển sinh - Flask Backend
Kiến trúc RAG:
  1. Người dùng gửi câu hỏi
  2. Embedding Model chuyển câu hỏi thành vector
  3. MongoDB Vector Search tìm thông tin liên quan
  4. Gemini LLM tổng hợp và sinh câu trả lời
  5. Flask trả kết quả về giao diện
"""

from flask import Flask, request, jsonify, render_template
from langchain_huggingface import HuggingFaceEmbeddings
from pymongo import MongoClient
import google.generativeai as genai

# ==============================
# CẤU HÌNH
# ==============================
MONGO_URI = "mongodb+srv://dungcony:vhErifXl4h1TwiMe@dungcony.qllqaij.mongodb.net/tuvantuyensinh?retryWrites=true&w=majority"
DB_NAME = "tuvantuyensinh"
COLLECTION_NAME = "documents"
VECTOR_INDEX_NAME = "vector_index"

# ⚠️ Thay bằng API key của bạn (lấy tại https://aistudio.google.com/apikey)
GEMINI_API_KEY = "YOUR_GEMINI_API_KEY"

# ==============================
# KHỞI TẠO
# ==============================
app = Flask(__name__)

# Khởi tạo Embedding Model (768 chiều)
print("🔄 Đang khởi tạo Embedding Model...")
embedding_model = HuggingFaceEmbeddings(
    model_name="sentence-transformers/paraphrase-multilingual-mpnet-base-v2",
    model_kwargs={"device": "cpu"},
    encode_kwargs={"normalize_embeddings": True}
)
print("✅ Embedding Model sẵn sàng!")

# Kết nối MongoDB
print("🔄 Đang kết nối MongoDB...")
client = MongoClient(MONGO_URI)
db = client[DB_NAME]
collection = db[COLLECTION_NAME]
print("✅ MongoDB đã kết nối!")

# Khởi tạo Gemini
print("🔄 Đang khởi tạo Gemini...")
genai.configure(api_key=GEMINI_API_KEY)
llm = genai.GenerativeModel("gemini-2.0-flash")
print("✅ Gemini sẵn sàng!")

print("\n" + "=" * 50)
print("🚀 Chatbot Tư vấn Tuyển sinh đã sẵn sàng!")
print("=" * 50)


# ==============================
# VECTOR SEARCH (Bước 3-4)
# ==============================
def vector_search(query: str, num_candidates: int = 150, limit: int = 4):
    """
    Tìm kiếm vector trong MongoDB (theo Hình 4.2.3):
    - index: "vector_index"
    - numCandidates: 150
    - limit: 4
    """
    query_vector = embedding_model.embed_query(query)

    pipeline = [
        {
            "$vectorSearch": {
                "index": VECTOR_INDEX_NAME,
                "path": "embedding",
                "queryVector": query_vector,
                "numCandidates": num_candidates,
                "limit": limit
            }
        },
        {
            "$project": {
                "_id": 0,
                "content": 1,
                "score": {"$meta": "vectorSearchScore"}
            }
        }
    ]

    results = list(collection.aggregate(pipeline))
    return results


# ==============================
# GEMINI LLM (Bước 5)
# ==============================
def generate_answer(query: str, context_docs: list) -> str:
    """
    Dùng Gemini tổng hợp thông tin từ context và sinh câu trả lời.
    """
    # Ghép context từ các chunks tìm được
    context = "\n\n---\n\n".join([doc["content"] for doc in context_docs])

    prompt = f"""Bạn là trợ lý tư vấn tuyển sinh đại học. Hãy trả lời câu hỏi của người dùng
dựa trên thông tin được cung cấp bên dưới. Trả lời bằng tiếng Việt, chính xác và dễ hiểu.

Nếu thông tin không đủ để trả lời, hãy nói rõ là bạn không có đủ thông tin.
Không bịa thông tin. Chỉ trả lời dựa trên dữ liệu được cung cấp.

THÔNG TIN THAM KHẢO:
{context}

CÂU HỎI: {query}

TRẢ LỜI:"""

    response = llm.generate_content(prompt)
    return response.text


# ==============================
# FLASK ROUTES
# ==============================
@app.route("/")
def index():
    """Trang chủ - giao diện chat"""
    return render_template("index.html")


@app.route("/chat", methods=["POST"])
def chat():
    """
    API endpoint xử lý câu hỏi:
    1. Nhận câu hỏi từ người dùng
    2. Vector search tìm context
    3. Gemini sinh câu trả lời
    """
    data = request.get_json()
    query = data.get("message", "").strip()

    if not query:
        return jsonify({"error": "Vui lòng nhập câu hỏi"}), 400

    try:
        # Bước 3-4: Vector Search tìm thông tin liên quan
        context_docs = vector_search(query, num_candidates=150, limit=4)

        if not context_docs:
            return jsonify({
                "answer": "Xin lỗi, tôi không tìm thấy thông tin liên quan. Vui lòng thử câu hỏi khác.",
                "sources": []
            })

        # Bước 5: Gemini sinh câu trả lời
        answer = generate_answer(query, context_docs)

        # Trả về kết quả
        sources = [
            {"content": doc["content"][:200], "score": round(doc.get("score", 0), 4)}
            for doc in context_docs
        ]

        return jsonify({
            "answer": answer,
            "sources": sources
        })

    except Exception as e:
        return jsonify({"error": f"Lỗi xử lý: {str(e)}"}), 500


# ==============================
# CHẠY SERVER
# ==============================
if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0", port=5000)
