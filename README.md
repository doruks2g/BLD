# 🛠️ bld - Minimalist C-Based Paket Yöneticisi

> [!CAUTION]
> **ÖNEMLİ:** Bu proje profesyonel kullanım için **DEĞİLDİR!** 
> El yapımı bir sistemdir; APT, Pacman veya DNF gibi ana paket yöneticilerinin yerini tutması amaçlanmamıştır. Tamamen bir **hobi ve öğrenme projesidir.**

`bld`, Linux sistemler için C diliyle geliştirilmiş, hafif, hızlı ve harici bir GitHub [reçete deposundan](https://github.com/doruks2g/bld-recipes) beslenen minimalist bir kaynak tabanlı (source-based) paket yöneticisidir.

Açık kaynak projeleri kendi bilgisayarınızda **ayrıştırılmış (staging)** alanlarda derler ve sistem genelinde kullanılabilmesi için `/usr/local/bin` altına otomatik **sembolik link (symlink)** bağlar.

## 🚀 Özellikler
*   **Hafif ve Hızlı:** Tamamen C ile yazılmıştır, sisteminizde hiçbir ek bağımlılık (Python, Node.js vb.) gerektirmez.
*   **🌐 Merkezi Depo Desteği:** `bld update` komutuyla uzak bir Git reposundaki reçeteleri bilgisayarınıza anında senkronize eder.
*   **📦 Temiz Kurulum (Staging):** Paketleri sistemin her yerine dağıtmak yerine `~/.bld/storage/pkgs/` altında izole eder. Bu sayede kaldırma işlemi (uninstall) çok temizdir.
*   **🛠️ Esnek Reçete Mimarisi:** `$PKG_DIR` ortam değişkeni desteği sayesinde en karmaşık kurulum adımlarını bile basitçe yönetir.

## 📥 Kurulum
Projeyi klonlayın ve aşağıdaki komutlarla derleyip sisteminize kurun:

```bash
# Kodları derleyin
gcc -O3 bld.c -o bld -Wno-format-truncation

# Binary'yi sistem dizinine kopyalayın
sudo cp bld /usr/local/bin/

⌨️ Kullanım
1. Reçete Deposunu Güncelleme
Uzak GitHub reposundaki en güncel .bld reçete dosyalarını çekmek için:
code
Bash
bld update
2. Paket Kurma
Bir paketi kaynak kodundan indirip derlemek ve kurmak için:
(Not: $HOME dizinini korumak için -E bayrağı gereklidir)
code
Bash
sudo -E bld install <paket_adı>
Örnek: sudo -E bld install neofetch
3. Paket Kaldırma
Kurulan bir paketi ve oluşturulan sembolik linkleri sistemden temizlemek için:
code
Bash
sudo -E bld remove <paket_adı>
📜 Reçete Yapısı (.bld)
Reçete dosyaları minimalisttir. Örnek bir neofetch.bld içeriği şu şekildedir:
code
Code
NAME:neofetch
VERSION:7.1.0
URL:https://github.com/dylanaraps/neofetch/archive/refs/tags/7.1.0.tar.gz
BUILD:true
INSTALL:cp neofetch $PKG_DIR/bin/
DEPS:
