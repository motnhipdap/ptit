from langchain_huggingface import HuggingFaceEmbeddings
from pymongo import MongoClient

# Cấu hình
MONGO_URI = "mongodb+srv://dungcony:vhErifXl4h1TwiMe@dungcony.qllqaij.mongodb.net/tuvantuyensinh?retryWrites=true&w=majority"
DB_NAME = "tuvantuyensinh"
COLLECTION_NAME = "documents"
VECTOR_INDEX_NAME = "vector_index"

# Kết nối MongoDB
client = MongoClient(MONGO_URI)
db = client[DB_NAME]
collection = db[COLLECTION_NAME]

# Khởi tạo embedding model (768 chiều)
print("Đang khởi tạo embedding model...")
embedding_model = HuggingFaceEmbeddings(
    model_name="sentence-transformers/paraphrase-multilingual-mpnet-base-v2",
    model_kwargs={"device": "cpu"},
    encode_kwargs={"normalize_embeddings": True}
)
print("✅ Model sẵn sàng!\n")


def vector_search(query: str, num_candidates: int = 150, limit: int = 4):
    """
    Tìm kiếm vector trong MongoDB theo cấu hình báo cáo (Hình 4.2.3):
    - index: "vector_index"
    - path: "embedding"
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



# Chế độ tương tác - nhập câu hỏi liên tục, gõ 'exit' để thoát
print("=" * 50)
print("TEST VECTOR SEARCH - Chatbot Tư vấn Tuyển sinh")
print("Gõ câu hỏi để tìm kiếm, gõ 'exit' để thoát")
print("=" * 50)

while True:
    query = input("\n🔍 Nhập câu hỏi: ").strip()
    if query.lower() in ("exit", "quit", "q"):
        break
    if not query:
        continue

    try:
        results = vector_search(query, num_candidates=150, limit=4)
        if results:
            for j, doc in enumerate(results):
                score = doc.get("score", 0)
                content = doc["content"][:200]
                print(f"  📄 Kết quả {j+1} (score: {score:.4f}): {content}...")
        else:
            print("  ⚠️  Không có kết quả.")
    except Exception as e:
        print(f"  ⚠️  Lỗi: {e}")

print("\n✅ Đã thoát!")
client.close()