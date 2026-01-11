#include "PrecompHeader.h"
#include "PluginManager.h"
#include "Processors/VSTProcessor.h"
#include <filesystem>
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

PluginManager::PluginManager() {
	// default paths
	mSearchPaths.push_back("C:\\Program Files\\VSTPlugins");
	mSearchPaths.push_back("C:\\Program Files (x86)\\VSTPlugins");
	mSearchPaths.push_back("C:\\Program Files\\Steinberg\\VSTPlugins");
}

void PluginManager::AddSearchPath(const std::string& path) {
	// avoid duplicates
	for (const auto& p : mSearchPaths) {
		if (p == path)
			return;
	}
	mSearchPaths.push_back(path);
}

void PluginManager::RemoveSearchPath(int index) {
	if (index >= 0 && index < (int)mSearchPaths.size()) {
		mSearchPaths.erase(mSearchPaths.begin() + index);
	}
}

void PluginManager::ScanPlugins() {
	std::lock_guard<std::mutex> lock(mMutex);
	mPlugins.clear();
	std::cout << "Scanning for Plugins...\n";

	for (const auto& pathStr : mSearchPaths) {
		fs::path root(pathStr);
		if (!fs::exists(root) || !fs::is_directory(root))
			continue;

		try {
			for (const auto& entry : fs::recursive_directory_iterator(root)) {
				if (entry.is_regular_file()) {
					std::string ext = entry.path().extension().string();
					std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

					if (ext == ".dll") {
						std::string fullPath = entry.path().string();

						// metadata scan
						VSTProcessor tempProc(fullPath);
						if (tempProc.Load()) {
							PluginInfo info;
							info.name = tempProc.GetName();
							info.path = fullPath;
							info.isSynth = tempProc.IsInstrument();
							info.vendor = ""; // VST2 metadata limitation

							mPlugins.push_back(info);
							std::cout << "Found: " << info.name << " (" << (info.isSynth ? "Inst" : "FX") << ")\n";
						}
					}
				}
			}
		} catch (const std::exception& e) {
			std::cout << "Error scanning directory " << pathStr << ": " << e.what() << "\n";
		}
	}
	std::cout << "Scan Complete. Found " << mPlugins.size() << " plugins.\n";
}
