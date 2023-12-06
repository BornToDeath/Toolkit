//
// Created by 郑伟 on 2021/5/14.
//

#include "data_wrapper.h"

#include <cstring>

#include "common/common_util.h"
#include "log.h"

#define TAG "DataWrapper"


bool DataWrapper::Wrap(int data_type, const BYTE *input_data, int input_len, BYTE **output_data, int &output_len) {
    output_len = input_len + kIntLength;
    *output_data = new BYTE[output_len]();
    BYTE *temp = *output_data;
    memcpy(temp, &data_type, kIntLength);
    memcpy(temp + kIntLength, input_data, input_len);
    return true;
}

bool DataWrapper::Parse(const BYTE *input_data, int input_len, int &data_type, BYTE **output_data, int &output_len) {
    if (input_len <= kIntLength) {
        Log::Warn(TAG, "DataWrapper::Parse() failed.");
        return false;
    }

    data_type = utils::common_util::Bytes2Int(input_data);
    output_len = input_len - kIntLength;
    *output_data = new BYTE[output_len]();
    memcpy(*output_data, input_data + kIntLength, output_len);
    return true;
}

int DataWrapper::ParseDataType(const BYTE *data, int start) {
    if (data == nullptr || start < 0) {
        return 0;
    }
    int len = data[start + 0] & 0xFF;
    len |= ((data[start + 1] << 8) & 0xFF00);
    len |= ((data[start + 2] << 16) & 0xFF0000);
    len |= ((data[start + 3] << 24) & 0xFF000000);
    return len;
}