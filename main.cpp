#include <iostream>
#include <filesystem>
#include <regex>
#include <string>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

void renameFile(const fs::path& filePath, const std::string& seasonNumber, const std::string& episodeNumber) {
	std::string filename = filePath.filename().string();
	std::string extension = filePath.extension().string();

	std::ostringstream formattedSeason, formattedEpisode;
	formattedSeason << "S" << std::setfill('0') << std::setw(2) << seasonNumber;
	formattedEpisode << "E" << std::setfill('0') << std::setw(2) << episodeNumber;

	std::string newFilename = std::regex_replace(filename, std::regex(R"(Season (\d+))", std::regex_constants::icase), formattedSeason.str());
	newFilename = std::regex_replace(newFilename, std::regex(R"(Episode (\d+))", std::regex_constants::icase), formattedEpisode.str());
	newFilename = std::regex_replace(newFilename, std::regex("S(\\d{2}) E"), "S$1E");

	fs::rename(filePath, filePath.parent_path() / newFilename);
}

void processFiles(const fs::path& directory) {
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (fs::is_directory(entry)) {
			processFiles(entry.path());
		}
		else if (fs::is_regular_file(entry)) {
			std::string filename = entry.path().filename().string();

			std::smatch seasonMatch;
			std::smatch episodeMatch;

			if (std::regex_search(filename, seasonMatch, std::regex(R"(Season (\d+))", std::regex_constants::icase))) {
				if (std::regex_search(filename, episodeMatch, std::regex(R"(Episode (\d+))", std::regex_constants::icase))) {
					int seasonNumber = std::stoi(seasonMatch[1].str());
					int episodeNumber = std::stoi(episodeMatch[1].str());

					renameFile(entry.path(), std::to_string(seasonNumber), std::to_string(episodeNumber));
				}
			}
		}
	}
}

int main() {
	fs::path currentPath = fs::current_path();

	processFiles(currentPath);

	std::cout << "Files renamed successfully." << std::endl;
	return 0;
}