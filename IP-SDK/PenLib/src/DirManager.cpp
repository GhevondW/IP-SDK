#include "DirManager.h"

using namespace pen;
using namespace std;
namespace fs = filesystem;
/**
 *
 *  Dir Manager Implementation
 *
 */

_CTOR_

DirManager::DirManager(const std::string& dir, const std::string& ext/* = ""*/)
    :_Dir(dir)
    , _Extension(ext)
    , _MainPath(_Dir)
{
    Init();
}

_DTOR_

DirManager::~DirManager()
{

}

_FUNC_

bool DirManager::IsDirExists() const
{
    return _bDirExists;
}

int DirManager::GetTotalFilesCount() const
{
    return _FilesPath.size();
}

int DirManager::GetExtFilesCount() const
{
    return GetFilesWithExt().size();
}

_GETS_

const string& DirManager::GetDir() const
{
    return _Dir;
}

const DirManager::FPaths& DirManager::GetFilesPath() const
{
    return _FilesPath;
}

const string& DirManager::GetExtension() const
{
    return _Extension;
}

const DirManager::FPaths& DirManager::GetFilesWithExt() const
{
    return _FilesPathNeeded;
}

_SETS_

void DirManager::SetDir(const std::string& value)
{
    Clear();
    _Dir = value;
    _MainPath = fs::path(_Dir);
    Init();
}

void DirManager::SetExt(const std::string& value)
{
    Clear();
    _Extension = value;
    Init();
}

void DirManager::Init()
{
    _bDirExists = fs::exists(_MainPath);
    if (_bDirExists) {
        const auto& ext = _MainPath.extension();
        if (ext == "") {
            for (const auto& entry : fs::directory_iterator(_MainPath))
            {
                const std::string& file_path = entry.path().string();
                fs::path p(file_path);
                std::string ext = p.extension().string();
                ext.replace(0, 1, "");
                _FilesPath.push_back(file_path);
                if (ext._Equal(_Extension)) {
                    _FilesPathNeeded.push_back(file_path);
                }
            }
        }
        else {
            _FilePath = _Dir;
            while (!_Dir.empty())
            {
                size_t str_len = _Dir.length();
                if (_Dir[str_len - 1] != '\\') {
                    _Dir.pop_back();
                }
                else {
                    break;
                }
            }
            _MainPath = fs::path(_Dir);
            Init();
        }
    }
}

void DirManager::Clear()
{
    _FilesPath.clear();
    _FilesPathNeeded.clear();
}

bool DirManager::CheckFilePathInDir(const std::string& FileName) const
{
    if (!FileName.empty()) {

        auto compare = [&](const std::string& fn) -> bool {
            std::size_t found = fn.find(FileName);
            if (found == std::string::npos)
            {
                return false;
            }
            return (fn.length() - found) == FileName.length();
        };

        auto res = std::find_if(_FilesPath.begin(), _FilesPath.end(), compare);
        return res != _FilesPath.end();
    }
    else if (!_FilePath.empty()) {
        return std::find(_FilesPath.begin(), _FilesPath.end(), _FilePath) != _FilesPath.end();
    }
    return false;
}

#if _DEBUG_
void DirManager::PrintClassInfo()
{
    std::cout << "------Dir Manager------" << std::endl;
    std::cout << "Dir:" << _Dir << std::endl;
    std::cout << "Ext:" << _Extension << std::endl;
    std::cout << "bExists:" << _bDirExists << std::endl;
    std::cout << "-----------------------" << std::endl;
}
#endif // _DEBUG_