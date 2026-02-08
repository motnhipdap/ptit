"""
Script crawl dữ liệu tuyển sinh từ website
Sử dụng: requests + BeautifulSoup
"""

# pip install requests beautifulsoup4

import requests
from bs4 import BeautifulSoup
import os
import time


def crawl_page(url, output_file):
    """Crawl nội dung text từ một trang web và lưu vào file .txt"""
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
    }

    try:
        response = requests.get(url, headers=headers, timeout=10)
        response.encoding = "utf-8"
        soup = BeautifulSoup(response.text, "html.parser")

        # Loại bỏ script, style, nav, footer
        for tag in soup(["script", "style", "nav", "footer", "header"]):
            tag.decompose()

        # Lấy nội dung text
        text = soup.get_text(separator="\n", strip=True)

        # Loại bỏ dòng trống thừa
        lines = [line.strip() for line in text.splitlines() if line.strip()]
        clean_text = "\n".join(lines)

        # Lưu vào file
        os.makedirs("data", exist_ok=True)
        with open(f"data/{output_file}", "w", encoding="utf-8") as f:
            f.write(clean_text)

        print(f"✅ Đã lưu: data/{output_file} ({len(clean_text)} ký tự)")
        return clean_text

    except Exception as e:
        print(f"❌ Lỗi khi crawl {url}: {e}")
        return None


# ==============================
# DANH SÁCH URL CẦN CRAWL
# ==============================
# Thay đổi URL theo trường bạn muốn lấy dữ liệu

urls = [
    # === PTIT - Học viện Công nghệ Bưu chính Viễn thông ===
    ("https://tuyensinh.ptit.edu.vn", "ptit_trangchu.txt"),
    ("https://tuyensinh.ptit.edu.vn/thong-bao-diem-chuan-trung-tuyen-vao-dai-hoc-he-chinh-quy-nam-2025/", "ptit_diemchuan_2025.txt"),
    ("https://tuyensinh.ptit.edu.vn/gioi-thieu/xem-diem-cac-nam-truoc/diem-trung-tuyen-2024/", "ptit_diemchuan_2024.txt"),
    ("https://tuyensinh.ptit.edu.vn/gioi-thieu/cau-hoi-thuong-gap/", "ptit_faq.txt"),
    ("https://tuyensinh.ptit.edu.vn/gioi-thieu/chinh-sach-hoc-bong/", "ptit_hocbong.txt"),
    ("https://tuyensinh.ptit.edu.vn/thong-bao-thong-tin-chi-tiet-cac-phuong-thuc-tuyen-sinh-dai-hoc-he-chinh-quy-nam-2026/", "ptit_phuongthuc_2026.txt"),

    # === BÁCH KHOA HÀ NỘI ===
    ("https://ts.hust.edu.vn", "bachkhoa_trangchu.txt"),
    ("https://ts.hust.edu.vn/tin-tuc/du-kien-phuong-an-tuyen-sinh-dai-hoc-2026-cua-bach-khoa-ha-noi", "bachkhoa_phuongan_2026.txt"),
    ("https://ts.hust.edu.vn/tin-tuc/diem-chuan-dai-hoc-bach-khoa-ha-noi-2024-diem-thi-dgtd-cao-nhat-83-82-diem-thi-tot-nghiep-thpt-cao-nhat-28-53", "bachkhoa_diemchuan_2024.txt"),
    ("https://ts.hust.edu.vn/training-cate/nganh-dao-tao-dai-hoc", "bachkhoa_nganhdaotao.txt"),
    ("https://ts.hust.edu.vn/tin-tuc/hoc-phi-dai-hoc-nam-2024-nien-khoa-2024-2025", "bachkhoa_hocphi.txt"),
]

if __name__ == "__main__":
    if not urls:
        print("⚠️  Chưa có URL nào được cấu hình.")
        print("Hãy thêm URL vào danh sách 'urls' trong file này.")
        print("\nVí dụ:")
        print('  ("https://tuyensinh.example.edu.vn/thong-tin", "thongtin.txt")')
        print("\nHoặc bạn có thể tự tạo file .txt trong thư mục data/ với nội dung")
        print("tuyển sinh copy từ website trường.")
    else:
        for url, filename in urls:
            crawl_page(url, filename)
            time.sleep(2)  # Delay 2s giữa các request để tránh bị chặn

        print(f"\n🎉 Đã crawl xong {len(urls)} trang!")
        print("Tiếp theo, chạy: python prepare_data.py")
