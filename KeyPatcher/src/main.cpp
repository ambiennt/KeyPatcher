namespace KeyData {

    inline constexpr std::string_view OLD_ROOT_PUBLIC_KEY{
        "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V"
    };

    inline constexpr std::string_view NEW_ROOT_PUBLIC_KEY{
        "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAECRXueJeTDqNRRgJi/vlRufByu/2G0i2Ebt6YMar5QX/R0DIIyrJMcUpruK4QveTfJSTp3Shlq4Gk34cD/4GUWwkv0DVuzeuB+tXija7HBxii03NHDbPAD0AKnLr2wdAp"
    };

    static_assert(OLD_ROOT_PUBLIC_KEY.length() == NEW_ROOT_PUBLIC_KEY.length());

} // namespace KeyData

using lps_array_t = std::array<int32_t, KeyData::NEW_ROOT_PUBLIC_KEY.size()>;

// create partial match table (longest proper prefix) for Knuth-Morris-Pratt algorithm
// https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
lps_array_t computeLPSArray(std::string_view pattern) {

    lps_array_t lps{};

    int32_t length = 0;
    size_t i = 1;
    lps[0] = 0;

    while (i < pattern.size()) {
        if (pattern[i] == pattern[length]) {
            ++length;
            lps[i] = length;
            ++i;
        }
        else {
            if (length != 0) {
                length = lps[length - 1];
            }
            else {
                lps[i] = 0;
                ++i;
            }
        }
    }

    return lps;
}
std::optional<size_t> findKeyOffset(std::fstream& file) {

    // reset file position
    file.seekg(0, std::ios::beg);

    const auto& pattern = KeyData::OLD_ROOT_PUBLIC_KEY; // treat the string literal as a signature to scan
    auto lps = computeLPSArray(pattern);

    int32_t j = 0; // index for pattern
    char c;

    for (size_t i = 0; file.get(c); ++i) {
        while ((j > 0) && (c != pattern[j])) {
            j = lps[j - 1];
        }

        if (c == pattern[j]) {
            ++j;
        }

        if (j == pattern.length()) {
            size_t foundOffset = (i - j + 1); // +1 because we want the starting index
            std::cout << "Found key offset at 0x" << std::hex << foundOffset << "!\n";
            return foundOffset;
        }
    }

    return std::nullopt;
}

bool patchFile(const std::filesystem::path& exePath) {

    std::fstream file{ exePath, std::ios::in | std::ios::out | std::ios::binary };
    if (!file.is_open()) {
        std::cerr <<
            "Failed to open file for patching! This may have happened because:\n"
            "- The Minecraft executable is stored in an OS-protected folder (such as WindowsApps) and has not been unlocked\n"
            "- This program does not have the necessary permissions to write to the Minecraft executable\n"
            "Note that even with sufficient write permissions, applying modifications to appx files requires sideloading!\n";
        return false;
    }

    auto keyOffset = findKeyOffset(file);
    if (!keyOffset) {
        std::cerr <<
            "Scan for old public key failed. This may have happened because:\n"
            "- the Minecraft executable has already been patched\n"
            "- the Minecraft executable predates Xbox authentication\n"
            "- the Minecraft executable uses a different (newer) authentication key\n";
        return false;
    }

    try {
        // go to the offset of the scanned key
        file.seekp(*keyOffset, std::ios::beg);

        // patch in new key
        file.write(KeyData::NEW_ROOT_PUBLIC_KEY.data(), KeyData::NEW_ROOT_PUBLIC_KEY.size());
    }
    catch (const std::ios_base::failure& ex) {
        std::cerr << "Failed to patch bytes: " << ex.what() << '\n';
        return false;
    }

    std::cout << "Successfully patched Mojang public Xbox authentication key!\n";
    return true;
}

/*
std::optional<std::filesystem::path> getWindowsAppsFolderPath() {
    wchar_t* buf = nullptr;
    size_t sz = 0;
    if ((_wdupenv_s(&buf, &sz, L"ProgramFiles") == 0) && (buf != nullptr)) {
        std::filesystem::path ret{ buf };
        ret /= L"WindowsApps";
        free(buf);
        return ret;
    }
    else {
        std::cerr << "Failed to retrieve %ProgramFiles% environment variable!\n";
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> getDefaultMinecraftFolder(const std::filesystem::path& baseDir) {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(baseDir)) {
            if (entry.is_directory()) {
                auto dirName = entry.path().filename().wstring();
                if (dirName.starts_with(L"Microsoft.MinecraftUWP_") && dirName.ends_with(L"_8wekyb3d8bbwe")) {
                    return entry.path();
                }
            }
        }
    }
    catch (std::filesystem::filesystem_error& ex) {
        std::cerr << "Could not locate default Minecraft folder: " << ex.what() << '\n';
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> getDefaultMinecraftPath() {
    auto appFolder = getWindowsAppsFolderPath();
    if (appFolder) {
        auto mcFolder = getDefaultMinecraftFolder(*appFolder);
        if (mcFolder) {
            return *mcFolder / L"Minecraft.Windows.exe";
        }
    }

    return std::nullopt;
}
*/

bool isValidExePath(const std::wstring& input) {
    if (!input.ends_with(L"Minecraft.Windows.exe")) {
        std::cerr << "File path did not resolve to a Minecraft executable!\n";
        return false;
    }

    std::filesystem::path path{ input };

    try {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Specified executable path doesn't exist on disk!\n";
            return false;
        }

        if (!std::filesystem::is_regular_file(path)) {
            std::cerr << "Specified path does not point to a file!\n";
            return false;
        }

        return true;
    }
    catch (std::filesystem::filesystem_error& ex) {
        std::cerr << "Failed to verify existence of Minecraft executable: " << ex.what() << '\n';
    }

    return false;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv, [[maybe_unused]] char** envp) {

    std::wstring input{};
    std::filesystem::path path{};
    bool patchedExe = false;

    while (!patchedExe) {

        std::cout << "Please enter the full Minecraft for Windows executable file path: ";
        std::getline(std::wcin, input);

        bool hasValidPath = false;

        /*if (input.empty()) { // wants to use WindowsApps path (default)
            auto opt = getDefaultMinecraftPath();
            if (opt) {
                path = *opt;
                hasValidPath = true;
            }
        }
        else {
            if (isValidExePath(input)) {
                path = input;
                hasValidPath = true;
            }
        }*/

        if (isValidExePath(input)) {
            path = input;
            hasValidPath = true;
        }

        if (hasValidPath) {
            patchedExe = patchFile(path);
        }
    }

    std::cout << "Press enter to continue...";
    std::cin.get();
    return 0;
}