#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "fileXferCallbacks.h"
#include "fileXferConf.h"
#include "fileXferUtils.h"

void files_list_gotten_cb(uint8_t files_num, uint8_t *files_names_arr) {
    char name_arr_temp[FXFER_FILE_NAME_LEN_MAX];
    uint16_t arr_offset = 0;
    printf("Respondents files list:\n");
    for (uint8_t i = 0; i < files_num; i++) {
        uint8_t len = files_names_arr[arr_offset];
        arr_offset++;
        memset(name_arr_temp, 0, FXFER_FILE_NAME_LEN_MAX);
        strncpy(name_arr_temp, &files_names_arr[arr_offset], len);
        arr_offset += len;

        printf("    - %s\n", name_arr_temp);
    }
}

void form_files_list_cb(uint8_t *payload_ptr, uint16_t free_space, uint16_t *payload_len) {
    DIR *d;
    uint16_t arr_offset = 1;
    uint8_t files_cnt = 0;

    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            uint8_t len = strlen(dir->d_name);
            if (free_space - arr_offset < 1 + len) {
                break;
            }
            payload_ptr[arr_offset] = len;
            arr_offset++;
            memcpy(&payload_ptr[arr_offset], dir->d_name, len);
            arr_offset += len;
            files_cnt++;


            printf("%s\n", dir->d_name);
        }
        payload_ptr[0] = files_cnt;
        *payload_len = arr_offset;
        closedir(d);
    }
}

void file_hash_gotten_cb(uint32_t *file_hash) {
    uint32_t crc32 = get_uint32_by_ptr(file_hash);
    printf("Gotten file hash: 0x%08X\n", crc32);
}

bool get_file_hash_cb(const char *file_name, uint32_t *file_hash) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        return false;
    }
    if (crc32_compute_file(file, file_hash) != 0) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

bool get_file_size_cb(const char *file_name, uint32_t *file_size) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        return false;
    }
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fclose(file);
    return true;
}

bool file_read_partial_cb(const char *file_name, uint32_t offset,
        uint32_t chunc_size, uint8_t *out_buf) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        return false;
    }
    fseek(file, offset, SEEK_SET);
    uint16_t num = fread(out_buf, 1, chunc_size, file);
    if (num != chunc_size) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

bool file_append_cb(const char *file_name, uint32_t chunc_size,
        uint8_t *in_buf, bool *eof_flag) {
    FILE *file = fopen(file_name, "a");
    if (file == NULL) {
        return false;
    }
    uint16_t num = fwrite(in_buf, 1, chunc_size, file);
    if (num != chunc_size) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}
