#pragma once

#include "AudioProcessor.h"
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <vector>

// built-in processors self-register at static initialization time
class ProcessorFactory {
public:
	using CreatorFunc = std::function<std::shared_ptr<AudioProcessor>()>;

	static ProcessorFactory& Instance() {
		static ProcessorFactory instance;
		return instance;
	}

	bool Register(const std::string& processorId, CreatorFunc creator, bool isInstrument = false) {
		if (mRegistry.find(processorId) != mRegistry.end()) {
			return false;
		}
		mRegistry[processorId] = creator;
		mIsInstrument[processorId] = isInstrument;
		return true;
	}
	std::shared_ptr<AudioProcessor> Create(const std::string& processorId) const {
		auto it = mRegistry.find(processorId);
		if (it != mRegistry.end()) {
			return it->second();
		}
		return nullptr;
	}
	bool IsRegistered(const std::string& processorId) const {
		return mRegistry.find(processorId) != mRegistry.end();
	}
	bool IsInstrument(const std::string& processorId) const {
		auto it = mIsInstrument.find(processorId);
		return it != mIsInstrument.end() && it->second;
	}
	std::vector<std::string> GetRegisteredProcessorIds() const {
		std::vector<std::string> ids;
		ids.reserve(mRegistry.size());
		for (const auto& pair : mRegistry) {
			ids.push_back(pair.first);
		}
		return ids;
	}
	std::vector<std::string> GetRegisteredInstrumentIds() const {
		std::vector<std::string> ids;
		for (const auto& pair : mIsInstrument) {
			if (pair.second) {
				ids.push_back(pair.first);
			}
		}
		return ids;
	}
private:
	ProcessorFactory() = default;
	~ProcessorFactory() = default;
	ProcessorFactory(const ProcessorFactory&) = delete;
	ProcessorFactory& operator=(const ProcessorFactory&) = delete;

	std::unordered_map<std::string, CreatorFunc> mRegistry;
	std::unordered_map<std::string, bool> mIsInstrument;
};

// automatic registration at static initialization:
// (place an instance of this in the processor's .cpp file)
class ProcessorRegistrar {
public:
	ProcessorRegistrar(const std::string& processorId,
					   ProcessorFactory::CreatorFunc creator,
					   bool isInstrument = false) {
		ProcessorFactory::Instance().Register(processorId, creator, isInstrument);
	}
};

#define REGISTER_PROCESSOR(ClassName, ProcessorId, IsInstrument) \
	namespace {                                                  \
		ProcessorRegistrar g##ClassName##Registrar(              \
			ProcessorId,                                         \
			[]() -> std::shared_ptr<AudioProcessor> {            \
				return std::make_shared<ClassName>();            \
			},                                                   \
			IsInstrument);                                       \
	}
