// File: system_info.c
// Simple cross-platform console program to gather OS info, C compiler presence/location/version, and geolocation.
// Build: gcc system_info.c -o system_info   (Windows: use appropriate compiler)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
const char *LOC_CMD = "where %s 2>NUL";
const char *VER_CMD = "ver";
#else
#define LOC_CMD "which %s 2>/dev/null"
const char *VER_CMD = "uname -sr";
#endif

// Run a shell command and capture stdout. Caller must free returned string.
char *run_cmd(const char *cmd) {
    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;
    size_t cap = 4096;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) { pclose(fp); return NULL; }
    while (!feof(fp)) {
        if (len + 1024 > cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); pclose(fp); return NULL; }
            buf = nb;
        }
        size_t r = fread(buf + len, 1, 1024, fp);
        len += r;
    }
    pclose(fp);
    if (len == 0) { free(buf); return NULL; }
    // trim trailing whitespace
    while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r' || buf[len-1] == ' ' || buf[len-1] == '\t')) len--;
    buf[len] = '\0';
    return buf;
}

// Try to get a JSON string field value like "key": "value"
char *json_get(const char *json, const char *key) {
    if (!json || !key) return NULL;
    char *pos = strstr(json, key);
    if (!pos) return NULL;
    pos = strchr(pos, ':');
    if (!pos) return NULL;
    // find first quote after colon
    char *q1 = strchr(pos, '\"');
    if (!q1) return NULL;
    char *q2 = q1 + 1;
    while (*q2 && *q2 != '\"') q2++;
    if (*q2 != '\"') return NULL;
    size_t len = q2 - (q1 + 1);
    char *out = malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, q1 + 1, len);
    out[len] = '\0';
    return out;
}

void print_row(const char *k, const char *v) {
    printf("%-25s : %s\n", k, v ? v : "(unknown)");
}

int main(void) {
    // OS type (compile-time)
    const char *os_type =
#ifdef _WIN32
        "Windows";
#elif __APPLE__
        "macOS";
#elif __linux__
        "Linux";
#elif __unix__
        "Unix";
#else
        "Unknown";
#endif

    // OS version/runtime
    char *os_version = run_cmd(VER_CMD);
#ifdef __APPLE__
    if (!os_version) {
        // macOS prefers sw_vers
        char *tmp = run_cmd("sw_vers -productVersion 2>/dev/null");
        if (tmp) { os_version = tmp; }
    }
#endif
#ifdef _WIN32
    // On Windows, try wmic for detailed info if available
    if (!os_version) {
        char *tmp = run_cmd("wmic os get Caption,Version /value 2>NUL");
        if (tmp) os_version = tmp;
    }
#endif

    // Check compilers
    const char *compilers[] = {"gcc", "clang", "cc", "cl"};
    char *found_comp = NULL;
    char *found_loc = NULL;
    char *found_ver = NULL;
    for (size_t i = 0; i < sizeof(compilers)/sizeof(compilers[0]); ++i) {
        const char *c = compilers[i];
        // find location
        char cmd_loc[256];
        snprintf(cmd_loc, sizeof(cmd_loc), LOC_CMD, c);
        char *loc = run_cmd(cmd_loc);
        if (!loc) continue;
        // get version (try common flags; redirect stderr to stdout)
        char cmd_ver[256];
#ifdef _WIN32
        if (strcmp(c, "cl") == 0) {
            snprintf(cmd_ver, sizeof(cmd_ver), "%s 2>&1", c); // cl prints to stderr sometimes
        } else {
            snprintf(cmd_ver, sizeof(cmd_ver), "%s --version 2>&1", c);
        }
#else
        snprintf(cmd_ver, sizeof(cmd_ver), "%s --version 2>&1", c);
#endif
        char *ver = run_cmd(cmd_ver);
        // Accept first compiler found
        found_comp = strdup(c);
        found_loc = loc; // take ownership of loc
        found_ver = ver; // may be NULL
        break;
    }

    // Geolocation via public IP geolocation services (try curl, wget)
    char *geo_json = NULL;
    const char *geo_cmds[] = {
        "curl -s https://ipinfo.io/json",
        "curl -s https://ipapi.co/json",
        "wget -qO- https://ipinfo.io/json",
        "wget -qO- https://ipapi.co/json"
#ifdef _WIN32
        , "powershell -Command \"(Invoke-RestMethod 'https://ipinfo.io/json' ) | ConvertTo-Json -Compress\""
#endif
    };
    for (size_t i = 0; i < sizeof(geo_cmds)/sizeof(geo_cmds[0]); ++i) {
        char *g = run_cmd(geo_cmds[i]);
        if (g) { geo_json = g; break; }
    }
    char *geo_city = NULL, *geo_region = NULL, *geo_country = NULL, *geo_loc = NULL;
    if (geo_json) {
        geo_city = json_get(geo_json, "city");
        geo_region = json_get(geo_json, "region");
        geo_country = json_get(geo_json, "country");
        geo_loc = json_get(geo_json, "loc");
    }

    // Print table-like output
    printf("========================================\n");
    printf("System Information\n");
    printf("========================================\n");
    print_row("OS Type", os_type);
    print_row("OS Version", os_version ? os_version : "(unknown)");
    printf("\n");
    printf("Compiler Information\n");
    printf("----------------------------------------\n");
    if (found_comp) {
        print_row("Compiler Found", found_comp);
        print_row("Install Location", found_loc ? found_loc : "(unknown)");
        print_row("Version Info", found_ver ? found_ver : "(unknown)");
    } else {
        print_row("Compiler Found", "No common C compiler found in PATH");
    }
    printf("\n");
    printf("Geolocation (based on public IP)\n");
    printf("----------------------------------------\n");
    if (geo_json) {
        print_row("City", geo_city ? geo_city : "(unknown)");
        print_row("Region", geo_region ? geo_region : "(unknown)");
        print_row("Country", geo_country ? geo_country : "(unknown)");
        print_row("Coords", geo_loc ? geo_loc : "(unknown)");
    } else {
        print_row("Geolocation", "Failed to retrieve (no curl/wget/powershell or network blocked)");
    }
    printf("========================================\n");

    // cleanup
    free(os_version);
    free(found_comp);
    free(found_loc);
    free(found_ver);
    free(geo_json);
    free(geo_city);
    free(geo_region);
    free(geo_country);
    free(geo_loc);
    return 0;
}