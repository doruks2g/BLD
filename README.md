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
