//
// Created by 郑伟 on 2021/5/14.
//

#include <cstring>
#include "DataWrapper.h"
#include "Log.h"
#include "Common/CommonUtil.h"

#define TAG "DataWrapper"


bool DataWrapper::wrap(int dataType, const BYTE *inputData, int inputLen, BYTE **outputData, int &outputLen) {
    outputLen = inputLen + UNIT_LENGTH;
    *outputData = new BYTE[outputLen]();
    BYTE *temp = *outputData;
    memcpy(temp, &dataType, UNIT_LENGTH);
    memcpy(temp + UNIT_LENGTH, inputData, inputLen);
    return true;
}

bool DataWrapper::parse(const BYTE *inputData, int inputLen, int &dataType, BYTE **outputData, int &outputLen) {
    if (inputLen <= UNIT_LENGTH) {
        Log::warn(TAG, "DataWrapper::parse() failed.");
        return false;
    }

    dataType = CommonUtil::bytes2Int(inputData);
    outputLen = inputLen - UNIT_LENGTH;
    *outputData = new BYTE[outputLen]();
    memcpy(*outputData, inputData + UNIT_LENGTH, outputLen);
    return true;
}

int DataWrapper::parseDataType(const BYTE *const data, int start) {
    if (data == nullptr || start < 0) {
        return 0;
    }
    int len = data[start + 0] & 0xFF;
    len |= ((data[start + 1] << 8) & 0xFF00);
    len |= ((data[start + 2] << 16) & 0xFF0000);
    len |= ((data[start + 3] << 24) & 0xFF000000);
    return len;
}