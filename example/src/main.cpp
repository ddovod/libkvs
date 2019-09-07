
#include <VirtualStorage.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "IStorage.hpp"

int main()
{
    auto storage = std::make_unique<kvs::VirtualStorage>();

    {  // Volume 1
        kvs::MountOptions options;
        options.storageMountPath = "A/B";
        options.volumeFilePath = "volume1.kvs";
        options.volumePath = "K/L";
        auto result = storage->mount(options);
        if (!result.isOk()) {
            std::abort();
        }
    }

    {  // Volume 2
        kvs::MountOptions options;
        options.storageMountPath = "A/B/C";
        options.volumeFilePath = "volume2.kvs";
        options.volumePath = "P/C/R";
        auto result = storage->mount(options);
        if (!result.isOk()) {
            std::abort();
        }
    }

    for (int i = 0; i < 100; i++) {
        auto key = "key_" + std::to_string(i);
        auto value = i;
        kvs::Status status;
        switch (i % 2) {
            case 0: {
                status = storage->putValue({key, "A/B"}, {value});
                break;
            }
            case 1: {
                status = storage->putValue({key, "A/B/C"}, {value});
                break;
            }
        }

        if (!status.isOk()) {
            switch (status.getFailReason()) {
                case kvs::Status::FailReason::kNodeNotFound: {
                    std::cout << "Fail putting value: node not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kKeyNotFound: {
                    std::cout << "Fail putting value: key not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kOk: {
                    break;
                }
            }
        }
    }

    for (int i = 1; i < 101; i++) {
        auto key = "key_" + std::to_string(i);
        kvs::Value value;
        auto status = storage->getValue({key, "A/B"}, &value);
        if (!status.isOk()) {
            switch (status.getFailReason()) {
                case kvs::Status::FailReason::kNodeNotFound: {
                    std::cout << "Fail getting value: node not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kKeyNotFound: {
                    std::cout << "Fail getting value: key not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kOk: {
                    break;
                }
            }
        }
    }

    for (int i = 1; i < 101; i++) {
        auto key = "key_" + std::to_string(i);
        kvs::Value value;
        auto status = storage->getValue({key, "A/B/C"}, &value);
        if (!status.isOk()) {
            switch (status.getFailReason()) {
                case kvs::Status::FailReason::kNodeNotFound: {
                    std::cout << "Fail getting value: node not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kKeyNotFound: {
                    std::cout << "Fail getting value: key not found: " << key << std::endl;
                    break;
                }
                case kvs::Status::FailReason::kOk: {
                    break;
                }
            }
        }
    }
}
