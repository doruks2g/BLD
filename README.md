
# bld - Minimalist C-Based Package Manager

PROFESYONEL KULLANIM İÇİN DEĞİLDİR!

EL YAPIMI BİR SİSTEMDİR APT VB. PAKET YÖNETİCİLERİ GİBİ TEK PAKET YÖNETİCİSİ OLARAK KULLANIMA UYGUN DEĞİLDİR BİR HOBİ PROJESİDİR

`bld`, Linux sistemler için C ile yazılmış, hafif, hızlı ve harici bir GitHub reçete deposundan (`repository`) beslenen minimalist bir kaynak tabanlı paket yöneticisidir. 

Açık kaynak projeleri kendi bilgisayarınızda ayrıştırılmış (`staging`) alanlarda derler ve sistem genelinde kullanılabilmesi için `/usr/local/bin` altına otomatik sembolik link (`symlink`) bağlar.

## Özellikler

- 🚀 **Hafif ve Hızlı:** C diliyle yazılmıştır, hiçbir ağır bağımlılığı yoktur.
- 🌐 **Merkezi Depo Desteği:** `bld update` komutuyla doğrudan uzak bir Git reposundaki reçeteleri bilgisayarınıza senkronize eder.
- 📦 **Temiz Kurulum (Staging):** Paketleri sistemin her yerine dağıtmak yerine `~/.bld/storage/pkgs/` altında izole eder, böylece kaldırması çok kolaydır.
- 🛠️ **Esnek Reçete Mimarisi:** `$PKG_DIR` ortam değişkeni desteği sayesinde karmaşık kurulum adımlarını kolayca yönetir.

---

## Kurulum

Projeyi klonlayın ve derleyerek sisteminize kurun:

```bash
# Kodları derleyin
gcc -O3 bld.c -o bld -Wno-format-truncation

# Binary'yi küresel dizine kopyalayın
sudo cp bld /usr/local/bin/

Kullanım
1. Reçete Deposunu Güncelleme
Uzak GitHub reposundaki en güncel .bld reçete dosyalarını yerel bilgisayarınıza çekmek için:

Bash
bld update
2. Paket Kurma
Reçetesi bulunan bir paketi kaynak kodundan indirip derlemek ve kurmak için ($HOME dizinini korumak için -E bayrağı gereklidir):

Bash
sudo -E bld install <paket_adı>
Örnek: sudo -E bld install neofetch

3. Paket Kaldırma
Kurulan bir paketi ve oluşturulan sembolik linkleri sistemden tamamen temizlemek için:

Bash
sudo -E bld remove <paket_adı>
Reçete Yapısı (.bld)
Reçete dosyaları çok basittir ve ana depoda <paket_adı>.bld şeklinde saklanır. Örnek neofetch.bld içeriği:

Plaintext
NAME:neofetch
VERSION:7.1.0
URL:https://github.com/dylanaraps/neofetch/archive/refs/tags/7.1.0.tar.gz
BUILD:true
INSTALL:cp neofetch $PKG_DIR/bin/
DEPS:
Bu proje açık kaynak topluluğuna katkı sağlamak ve Linux sistem programlama mantığını anlamak için geliştirilmiştir. 🌟
