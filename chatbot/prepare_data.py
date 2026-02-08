"""
Pipeline chuẩn bị dữ liệu cho Chatbot Tư vấn Tuyển sinh (RAG)
Quy trình: Load Document -> Chunking -> Embedding -> Lưu MongoDB
Theo cấu trúc báo cáo thực tập:
  - chunk_size=800, chunk_overlap=400
  - MongoDB schema: { _id, content, embedding (Array 768) }
"""

# ==============================
# 1. CÀI ĐẶT THƯ VIỆN
# ==============================

#pip install langchain langchain-community langchain-huggingface sentence-transformers pymongo


from langchain_community.document_loaders import TextLoader, DirectoryLoader
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_huggingface import HuggingFaceEmbeddings
from pymongo import MongoClient

# ==============================
# CẤU HÌNH MONGODB
# ==============================
MONGO_URI = "mongodb+srv://dungcony:vhErifXl4h1TwiMe@dungcony.qllqaij.mongodb.net/tuvantuyensinh?retryWrites=true&w=majority"
DB_NAME = "tuvantuyensinh"
COLLECTION_NAME = "documents"
VECTOR_INDEX_NAME = "vector_index"

# ==============================
# 2. LOAD DOCUMENT - Tải văn bản 
# ==============================
print("=" * 50)
print("BƯỚC 1: Load Document")
print("=" * 50)

# Load tất cả file .txt trong thư mục data/
loader_dir = DirectoryLoader("data/", glob="**/*.txt", loader_cls=TextLoader,
                              loader_kwargs={"encoding": "utf-8"})
documents = loader_dir.load()

print(f"Số lượng tài liệu đã tải: {len(documents)}")
for i, doc in enumerate(documents):
    print(f"  - File {i+1}: {doc.metadata.get('source', 'N/A')} ({len(doc.page_content)} ký tự)")

# ==============================
# 3. CHUNKING - Chia nhỏ văn bản
# ==============================
print("\n" + "=" * 50)
print("BƯỚC 2: Chunking Document (chunk_size=800, overlap=400)")
print("=" * 50)

# Chia văn bản thành các đoạn nhỏ theo cấu hình báo cáo (Hình 4.1.1):
# - chunk_size=800: mỗi đoạn tối đa 800 ký tự
# - chunk_overlap=400: trùng lặp 400 ký tự giữa các đoạn liền kề
text_splitter = RecursiveCharacterTextSplitter(
    chunk_size=800,
    chunk_overlap=400
)
texts = text_splitter.split_documents(documents)

print(f"Số lượng chunks sau khi chia: {len(texts)}")
for i, chunk in enumerate(texts[:5]):  # In 5 chunk đầu tiên
    print(f"\n--- Chunk {i+1} ({len(chunk.page_content)} ký tự) ---")
    print(chunk.page_content[:150] + "...")
if len(texts) > 5:
    print(f"\n... và {len(texts) - 5} chunks khác")

# ==============================
# 4. EMBEDDING MODEL
# ==============================
print("\n" + "=" * 50)
print("BƯỚC 3: Khởi tạo Embedding Model (768 chiều)")
print("=" * 50)

# Model sentence-transformers cho embedding 768 chiều (theo báo cáo Hình 4.1.2)
embedding_model = HuggingFaceEmbeddings(
    model_name="sentence-transformers/paraphrase-multilingual-mpnet-base-v2",
    model_kwargs={"device": "cpu"},
    encode_kwargs={"normalize_embeddings": True}
)

# Kiểm tra kích thước vector
test_vector = embedding_model.embed_query("test")
print(f"Embedding model: paraphrase-multilingual-MiniLM-L12-v2")
print(f"Kích thước vector: {len(test_vector)} chiều")

# ==============================
# 5. LƯU VÀO MONGODB
# ==============================
print("\n" + "=" * 50)
print("BƯỚC 4: Lưu vào MongoDB")
print("=" * 50)

# Kết nối MongoDB
client = MongoClient(MONGO_URI)
db = client[DB_NAME]
collection = db[COLLECTION_NAME]

# Xóa dữ liệu cũ (nếu muốn chạy lại từ đầu)
old_count = collection.count_documents({})
if old_count > 0:
    print(f"Xóa {old_count} bản ghi cũ...")
    collection.delete_many({})

# Tạo embedding và lưu từng chunk vào MongoDB
# Schema theo báo cáo (Hình 4.1.2):
# { _id: ObjectId, content: String, embedding: Array(768) }
print(f"Đang tạo embedding và lưu {len(texts)} chunks vào MongoDB...")

mongo_docs = []
for i, chunk in enumerate(texts):
    # Tạo vector embedding cho đoạn văn bản
    vector = embedding_model.embed_query(chunk.page_content)

    # Tạo document theo schema báo cáo
    mongo_doc = {
        "content": chunk.page_content,
        "embedding": vector
    }
    mongo_docs.append(mongo_doc)

    if (i + 1) % 10 == 0 or (i + 1) == len(texts):
        print(f"  Đã xử lý: {i+1}/{len(texts)} chunks")

# Bulk insert vào MongoDB
collection.insert_many(mongo_docs)
print(f"\n✅ Đã lưu {len(mongo_docs)} documents vào MongoDB")
print(f"   Database: {DB_NAME}")
print(f"   Collection: {COLLECTION_NAME}")
print(f"   Schema: {{ _id, content, embedding (Array {len(test_vector)}) }}")

# ==============================
# 6. TẠO VECTOR INDEX (Hướng dẫn)
# ==============================
print("\n" + "=" * 50)
print("BƯỚC 5: Tạo Vector Search Index trên MongoDB Atlas")
print("=" * 50)

print("""
⚠️  Bạn cần tạo Vector Search Index trên MongoDB Atlas:

1. Truy cập MongoDB Atlas → Database → Collection → Search Indexes
2. Chọn "Create Search Index" → JSON Editor
3. Đặt tên index: "vector_index"
4. Dán cấu hình sau:

{
  "fields": [
    {
      "type": "vector",
      "path": "embedding",
      "numDimensions": 768,
      "similarity": "cosine"
    }
  ]
}

5. Nhấn "Create Search Index"
""")

# ==============================
# 7. TEST TÌM KIẾM (Vector Search)
# ==============================
print("=" * 50)
print("BƯỚC 6: Test Vector Search (theo Hình 4.2.3)")
print("=" * 50)


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


# Thử truy vấn
queries = [
    "Điểm chuẩn ngành Công nghệ thông tin là bao nhiêu?",
    "Học phí trường bao nhiêu?",
    "Có những phương thức tuyển sinh nào?",
]

try:
    for query in queries:
        print(f"\n🔍 Câu hỏi: {query}")
        results = vector_search(query, num_candidates=150, limit=4)
        if results:
            for j, doc in enumerate(results):
                score = doc.get("score", 0)
                content = doc["content"][:120]
                print(f"  📄 Kết quả {j+1} (score: {score:.4f}): {content}...")
        else:
            print("  ⚠️  Không có kết quả. Hãy kiểm tra Vector Search Index đã được tạo chưa.")
except Exception as e:
    print(f"\n⚠️  Chưa thể test vector search: {e}")
    print("   → Hãy tạo Vector Search Index trên MongoDB Atlas trước (xem hướng dẫn ở Bước 5)")

print("\n✅ Pipeline hoàn tất! MongoDB đã sẵn sàng cho chatbot RAG.")

# Đóng kết nối
client.close()
