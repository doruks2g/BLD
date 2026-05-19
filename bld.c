#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

#define MAX_NAME    128
#define MAX_VERSION 64
#define MAX_URL     512
#define MAX_LINE    2048
#define PATH_LIMIT  4096
#define CMD_LIMIT   8192

#define DEFAULT_REPO "https://github.com/doruks2g/bld-recipes.git"

char BASE_DIR[PATH_LIMIT], RECIPES_DIR[PATH_LIMIT], CACHE_DIR[PATH_LIMIT];
char SRC_DIR[PATH_LIMIT], PKGS_DIR[PATH_LIMIT], BIN_DIR[PATH_LIMIT];

typedef struct {
    char name[MAX_NAME];
    char version[MAX_VERSION];
    char url[MAX_URL];
    char hash[128];
    char build_cmd[MAX_LINE];
    char install_cmd[MAX_LINE];
    char deps[MAX_LINE];
} Recipe;

int run_cmd(const char *cmd) {
    printf("\033[1;34m[RUN]\033[0m %s\n", cmd);
    return system(cmd);
}

void setup_paths() {
    const char *homedir = getenv("HOME");
    if (!homedir) homedir = getpwuid(getuid())->pw_dir;
    snprintf(BASE_DIR,    PATH_LIMIT, "%s/.bld", homedir);
    snprintf(RECIPES_DIR, PATH_LIMIT, "%s/recipes", BASE_DIR);
    snprintf(CACHE_DIR,   PATH_LIMIT, "%s/storage/cache", BASE_DIR);
    snprintf(SRC_DIR,     PATH_LIMIT, "%s/storage/src", BASE_DIR);
    snprintf(PKGS_DIR,    PATH_LIMIT, "%s/storage/pkgs", BASE_DIR);
    snprintf(BIN_DIR,     PATH_LIMIT, "%s/.local/bin", homedir);
}

void init_folders() {
    char cmd[CMD_LIMIT];
    snprintf(cmd, CMD_LIMIT, "mkdir -p '%s' '%s' '%s' '%s' '%s'", 
             CACHE_DIR, SRC_DIR, PKGS_DIR, BIN_DIR, RECIPES_DIR);
    system(cmd);
}

int load_recipe(const char *pkg_name, Recipe *recipe) {
    char recipe_path[PATH_LIMIT];
    snprintf(recipe_path, PATH_LIMIT, "%s/%s.bld", RECIPES_DIR, pkg_name);
    FILE *file = fopen(recipe_path, "r");
    if (!file) return 0;
    char line[MAX_LINE];
    memset(recipe, 0, sizeof(Recipe));
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strncmp(line, "NAME:", 5) == 0) strcpy(recipe->name, line + 5);
        else if (strncmp(line, "VERSION:", 8) == 0) strcpy(recipe->version, line + 8);
        else if (strncmp(line, "URL:", 4) == 0) strcpy(recipe->url, line + 4);
        else if (strncmp(line, "HASH:", 5) == 0) strcpy(recipe->hash, line + 5);
        else if (strncmp(line, "BUILD:", 6) == 0) strcpy(recipe->build_cmd, line + 6);
        else if (strncmp(line, "INSTALL:", 8) == 0) strcpy(recipe->install_cmd, line + 8);
        else if (strncmp(line, "DEPS:", 5) == 0) strcpy(recipe->deps, line + 5);
    }
    fclose(file);
    return 1;
}

int is_installed(const char *name, const char *version) {
    char path[PATH_LIMIT];
    snprintf(path, PATH_LIMIT, "%s/%s-%s", PKGS_DIR, name, version);
    return access(path, F_OK) == 0;
}

void remove_package(const char *pkg_name) {
    Recipe recipe;
    if (!load_recipe(pkg_name, &recipe)) {
        char link_path[PATH_LIMIT];
        snprintf(link_path, PATH_LIMIT, "%s/%s", BIN_DIR, pkg_name);
        unlink(link_path);
        return;
    }
    char link_path[PATH_LIMIT], pkg_path[PATH_LIMIT], cmd[CMD_LIMIT];
    snprintf(link_path, PATH_LIMIT, "%s/%s", BIN_DIR, recipe.name);
    snprintf(pkg_path, PATH_LIMIT, "%s/%s-%s", PKGS_DIR, recipe.name, recipe.version);
    unlink(link_path);
    snprintf(cmd, CMD_LIMIT, "rm -rf '%s'", pkg_path);
    run_cmd(cmd);
    printf("\033[1;32m[!] %s kaldirildi.\033[0m\n", pkg_name);
}

void install_package(const char *pkg_name, int force_upgrade) {
    init_folders();
    Recipe recipe;
    if (!load_recipe(pkg_name, &recipe)) {
        printf("\033[1;31m[-] Recete bulunamadi: %s\033[0m\n", pkg_name);
        return;
    }
    if (!force_upgrade && is_installed(recipe.name, recipe.version)) {
        printf("\033[1;33m[i] %s zaten kurulu (v%s).\033[0m\n", recipe.name, recipe.version);
        return;
    }
    if (strlen(recipe.deps) > 0) {
        char *deps_copy = strdup(recipe.deps);
        char *dep = strtok(deps_copy, ", ");
        while (dep) {
            install_package(dep, 0);
            dep = strtok(NULL, ", ");
        }
        free(deps_copy);
    }
    char cmd[CMD_LIMIT], archive[PATH_LIMIT], build_path[PATH_LIMIT], pkg_dest[PATH_LIMIT];
    snprintf(build_path, PATH_LIMIT, "%s/%s", SRC_DIR, recipe.name);
    snprintf(pkg_dest, PATH_LIMIT, "%s/%s-%s", PKGS_DIR, recipe.name, recipe.version);
    if (strstr(recipe.url, ".git")) {
        snprintf(cmd, CMD_LIMIT, "rm -rf '%s' && git clone --depth 1 '%s' '%s'", build_path, recipe.url, build_path);
        run_cmd(cmd);
    } else {
        snprintf(archive, PATH_LIMIT, "%s/%s-%s.src", CACHE_DIR, recipe.name, recipe.version);
        if (access(archive, F_OK) != 0) {
            snprintf(cmd, CMD_LIMIT, "wget -O '%s' '%s'", archive, recipe.url);
            run_cmd(cmd);
        }
        if (strlen(recipe.hash) > 0) {
            snprintf(cmd, CMD_LIMIT, "echo '%s  %s' | sha256sum -c", recipe.hash, archive);
            if (run_cmd(cmd) != 0) {
                printf("\033[1;31m[-] HASH dogrulamasi basarisiz!\033[0m\n");
                return;
            }
        }
        snprintf(cmd, CMD_LIMIT, "rm -rf '%s' && mkdir -p '%s' && tar -xf '%s' -C '%s' --strip-components=1 2>/dev/null || unzip -q '%s' -d '%s'", build_path, build_path, archive, build_path, archive, build_path);
        run_cmd(cmd);
    }
    setenv("PKG_DIR", pkg_dest, 1);
    snprintf(cmd, CMD_LIMIT, "mkdir -p '%s/bin' && cd '%s' && %s && %s", pkg_dest, build_path, recipe.build_cmd, recipe.install_cmd);
    if (run_cmd(cmd) == 0) {
        char target_bin[PATH_LIMIT], link_path[PATH_LIMIT];
        snprintf(target_bin, PATH_LIMIT, "%s/bin/%s", pkg_dest, recipe.name);
        snprintf(link_path, PATH_LIMIT, "%s/%s", BIN_DIR, recipe.name);
        unlink(link_path);
        symlink(target_bin, link_path);
        printf("\033[1;32m[+] %s v%s kuruldu.\033[0m\n", recipe.name, recipe.version);
    }
}

void list_packages() {
    char cmd[CMD_LIMIT];
    snprintf(cmd, CMD_LIMIT, "ls -1 %s", PKGS_DIR);
    printf("\033[1;36m--- Kurulu Paketler ---\033[0m\n");
    system(cmd);
}

void self_upgrade() {
    printf("\033[1;33m[i] bld guncelleniyor...\033[0m\n");
    char cmd[CMD_LIMIT];
    snprintf(cmd, CMD_LIMIT, "cd %s/.. && git pull && gcc bld.c -o bld && mv bld %s/bld", RECIPES_DIR, BIN_DIR);
    run_cmd(cmd);
}

int main(int argc, char *argv[]) {
    setup_paths();
    if (argc < 2) {
        printf("Kullanim: bld <update|install|remove|list|upgrade|self-upgrade> [paket]\n");
        return 1;
    }
    if (strcmp(argv[1], "update") == 0) {
        char cmd[CMD_LIMIT];
        if (access(RECIPES_DIR, F_OK) == 0) snprintf(cmd, CMD_LIMIT, "cd '%s' && git pull", RECIPES_DIR);
        else snprintf(cmd, CMD_LIMIT, "git clone %s '%s'", DEFAULT_REPO, RECIPES_DIR);
        run_cmd(cmd);
    } 
    else if (strcmp(argv[1], "install") == 0 && argc == 3) install_package(argv[2], 0);
    else if (strcmp(argv[1], "upgrade") == 0 && argc == 3) install_package(argv[2], 1);
    else if (strcmp(argv[1], "remove") == 0 && argc == 3) remove_package(argv[2]);
    else if (strcmp(argv[1], "list") == 0) list_packages();
    else if (strcmp(argv[1], "self-upgrade") == 0) self_upgrade();
    else printf("Gecersiz komut.\n");
    return 0;
}
