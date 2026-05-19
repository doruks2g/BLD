
# bld - Minimalist C-Based Package Manager

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
