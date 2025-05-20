#include "Utils.h"


int endsWithStr(const char *base,  const char *str) {

    // 检查输入是否为NULL
    if (base == NULL || str == NULL) {
        return 0;
    }

    size_t base_len = strlen(base);
    size_t str_len = strlen(str);

    // 如果str的长度大于base，则base不可能以str结尾
    if (str_len > base_len) {
        return 0;
    }

    // 计算需要比较的位置
    const char *base_end = base + (base_len - str_len);

    // 比较结尾部分
    return strncmp(base_end, str, str_len) == 0 ? 1 : 0;
}

int hasStr(const char *base, const char *str) {
    // 检查输入是否为NULL
    if (base == NULL || str == NULL) {
        return 0;
    }

    size_t base_len = strlen(base);
    size_t str_len = strlen(str);

    // 如果str的长度大于base，则base不可能包含str
    if (str_len > base_len) {
        return 0;
    }

    // 遍历base字符串，查找是否包含str
    for (size_t i = 0; i <= base_len - str_len; i++) {
        // 比较从当前位置开始的子串是否与str匹配
        if (strncmp(base + i, str, str_len) == 0) {
            return 1; // 找到匹配，返回1
        }
    }

    return 0; // 未找到匹配，返回0
}

