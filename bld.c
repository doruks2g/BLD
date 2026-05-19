#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

#define MAX_NAME    128
#define MAX_VERSION 64
#define MAX_URL     512
#define MAX_LINE    1024

#define PATH_LIMIT     4096
#define CMD_LIMIT      20000


#define RECIPE_REPO_URL "https://github.com/doruks2g/bld-recipes.git"

char BASE_DIR[PATH_LIMIT];
char RECIPES_DIR[PATH_LIMIT];
char CACHE_DIR[PATH_LIMIT];
char SRC_DIR[PATH_LIMIT];
char PKGS_DIR[PATH_LIMIT];

typedef struct {
    char name[MAX_NAME];
    char version[MAX_VERSION];
    char url[MAX_URL];
    char build_cmd[MAX_LINE];
    char install_cmd[MAX_LINE];
} Recipe;

void setup_paths() {
    const char *homedir = getenv("HOME");
    if (homedir == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    
    snprintf(BASE_DIR, sizeof(BASE_DIR), "%.4000s/.bld", homedir);
    snprintf(RECIPES_DIR, sizeof(RECIPES_DIR), "%.4000s/recipes", BASE_DIR);
    snprintf(CACHE_DIR, sizeof(CACHE_DIR), "%.4000s/storage/cache", BASE_DIR);
    snprintf(SRC_DIR, sizeof(SRC_DIR), "%.4000s/storage/src", BASE_DIR);
    snprintf(PKGS_DIR, sizeof(PKGS_DIR), "%.4000s/storage/pkgs", BASE_DIR);
}

void init_folders() {
    char cmd[CMD_LIMIT];
    snprintf(cmd, sizeof(cmd), "mkdir -p '%.4095s' '%.4095s' '%.4095s'", 
             CACHE_DIR, SRC_DIR, PKGS_DIR);
    if (system(cmd) != 0) { }
    
    snprintf(cmd, sizeof(cmd), "mkdir -p '%.4095s'", BASE_DIR);
    if (system(cmd) != 0) { }
}

void update_recipes() {
    init_folders();
    char cmd[CMD_LIMIT];
    char git_check_path[PATH_LIMIT];
    
    snprintf(git_check_path, sizeof(git_check_path), "%.4000s/.git", RECIPES_DIR);
    
    if (access(git_check_path, F_OK) == 0) {
        printf("[+] Reçete deposu güncelleniyor (git pull)...\n");
        snprintf(cmd, sizeof(cmd), "cd '%.4000s' && git pull", RECIPES_DIR);
    } else {
        printf("[+] Reçete deposu ilk kez indiriliyor (git clone)...\n");
        snprintf(cmd, sizeof(cmd), "rm -rf '%.4000s'", RECIPES_DIR);
        if (system(cmd) != 0) {}
        
        snprintf(cmd, sizeof(cmd), "git clone %.512s '%.4000s'", RECIPE_REPO_URL, RECIPES_DIR);
    }

    if (system(cmd) == 0) {
        printf("[+++] Reçeteler başarıyla güncellendi!\n");
    } else {
        fprintf(stderr, "[-] Hata: Reçete deposu güncellenemedi. İnternet bağlantınızı veya git kurulumunu kontrol edin.\n");
    }
}

int load_recipe(const char *pkg_name, Recipe *recipe) {
    char recipe_path[PATH_LIMIT];
    snprintf(recipe_path, sizeof(recipe_path), "%.4000s/%.128s.bld", RECIPES_DIR, pkg_name);

    FILE *file = fopen(recipe_path, "r");
    if (!file) {
        printf("[-] Hata: '%s' reçetesi bulunamadı!\n", recipe_path);
        printf("[-] Önce 'bld update' çalıştırmayı deneyin veya reçeteyi kontrol edin.\n");
        return 0;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "NAME:", 5) == 0)      strncpy(recipe->name, line + 5, sizeof(recipe->name) - 1);
        else if (strncmp(line, "VERSION:", 8) == 0) strncpy(recipe->version, line + 8, sizeof(recipe->version) - 1);
        else if (strncmp(line, "URL:", 4) == 0)      strncpy(recipe->url, line + 4, sizeof(recipe->url) - 1);
        else if (strncmp(line, "BUILD:", 6) == 0)    strncpy(recipe->build_cmd, line + 6, sizeof(recipe->build_cmd) - 1);
        else if (strncmp(line, "INSTALL:", 8) == 0)  strncpy(recipe->install_cmd, line + 8, sizeof(recipe->install_cmd) - 1);
    }

    fclose(file);
    return 1;
}

void install_package(const char *pkg_name) {
    init_folders();
    Recipe recipe;
    memset(&recipe, 0, sizeof(Recipe));
    
    if (!load_recipe(pkg_name, &recipe)) return;

    char cmd[CMD_LIMIT];

    
    printf("[+] %s (%s) indiriliyor...\n", recipe.name, recipe.version);
    snprintf(cmd, sizeof(cmd), "wget -q --show-progress -O '%.4000s/%.128s-%.64s.tar.gz' '%.512s'", 
             CACHE_DIR, recipe.name, recipe.version, recipe.url);
    if (system(cmd) != 0) {
        fprintf(stderr, "[-] Hata: Kaynak kod indirilemedi!\n");
        return;
    }

    
    printf("[+] Arşiv açılıyor...\n");
    snprintf(cmd, sizeof(cmd), "tar -xf '%.4000s/%.128s-%.64s.tar.gz' -C '%.4000s/'", 
             CACHE_DIR, recipe.name, recipe.version, SRC_DIR);
    if (system(cmd) != 0) {
        fprintf(stderr, "[-] Hata: Arşiv açılamadı!\n");
        return;
    }

   
    char specific_src_dir[PATH_LIMIT];
    snprintf(specific_src_dir, sizeof(specific_src_dir), "%.4000s/%.128s-%.64s", SRC_DIR, recipe.name, recipe.version);
    printf("[+] Derleniyor: %s\n", recipe.build_cmd);
    snprintf(cmd, sizeof(cmd), "cd '%.4000s' && %.1024s", specific_src_dir, recipe.build_cmd);
    if (system(cmd) != 0) {
        fprintf(stderr, "[-] Hata: Derleme başarısız oldu!\n");
        return;
    }

    
    char specific_pkg_dir[PATH_LIMIT];
    snprintf(specific_pkg_dir, sizeof(specific_pkg_dir), "%.4000s/%.128s-%.64s", PKGS_DIR, recipe.name, recipe.version);
    
    snprintf(cmd, sizeof(cmd), "mkdir -p '%.4000s/bin'", specific_pkg_dir);
    if (system(cmd) != 0) {}

    printf("[+] Paket dizinine taşınıyor...\n");
    
    
    setenv("PKG_DIR", specific_pkg_dir, 1);
    
   
    snprintf(cmd, sizeof(cmd), "cd '%.4000s' && %.1024s", specific_src_dir, recipe.install_cmd);
    
    if (system(cmd) != 0) {
        fprintf(stderr, "[-] Hata: Kurulum adımları başarısız!\n");
        return;
    }


    char target_binary[PATH_LIMIT];
    char system_link[PATH_LIMIT];
    snprintf(target_binary, sizeof(target_binary), "%.4000s/bin/%.128s", specific_pkg_dir, recipe.name);
    snprintf(system_link, sizeof(system_link), "/usr/local/bin/%.128s", recipe.name);

    if (access(target_binary, F_OK) == 0) {
        printf("[+] Sembolik link güncelleniyor: %s\n", system_link);
        unlink(system_link); 
        if (symlink(target_binary, system_link) == 0) {
            printf("[+++] %s %s başarıyla kuruldu! Terminalden '%s' yazarak çalıştırabilirsin.\n", 
                   recipe.name, recipe.version, recipe.name);
        } else {
            perror("[-] Symlink hatası (Sudo yetkisi gerekebilir)");
        }
    } else {
        fprintf(stderr, "[-] Hata: Derlenen binary bulunamadı (%s)\n", target_binary);
    }
}

void remove_package(const char *pkg_name) {
    Recipe recipe;
    memset(&recipe, 0, sizeof(Recipe));
    if (!load_recipe(pkg_name, &recipe)) return;

    char system_link[PATH_LIMIT];
    char cmd[CMD_LIMIT];
    snprintf(system_link, sizeof(system_link), "/usr/local/bin/%.128s", pkg_name);

    printf("[+] Paket sistemden kaldırılıyor: %s\n", pkg_name);
    unlink(system_link);

    snprintf(cmd, sizeof(cmd), "rm -rf '%.4000s/%.128s-%.64s' '%.4000s/%.128s-%.64s'", 
             PKGS_DIR, recipe.name, recipe.version, SRC_DIR, recipe.name, recipe.version);
    if (system(cmd) != 0) {
        fprintf(stderr, "[-] Hata: Dosyalar temizlenirken bir sorun oluştu!\n");
    }

    printf("[+++] %s başarıyla sistemden silindi.\n", pkg_name);
}

int main(int argc, char *argv[]) {
    setup_paths();

    if (argc < 2) {
        printf("Kullanım:\n  bld update\n  bld install <paket_adı>\n  bld remove  <paket_adı>\n");
        return 1;
    }

    if (strcmp(argv[1], "update") == 0) {
        update_recipes();
    } else if (strcmp(argv[1], "install") == 0) {
        if (argc < 3) { printf("[-] Hata: Paket adı belirtmelisiniz!\n"); return 1; }
        install_package(argv[2]);
    } else if (strcmp(argv[1], "remove") == 0) {
        if (argc < 3) { printf("[-] Hata: Paket adı belirtmelisiniz!\n"); return 1; }
        remove_package(argv[2]);
    } else {
        printf("[-] Geçersiz komut: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
