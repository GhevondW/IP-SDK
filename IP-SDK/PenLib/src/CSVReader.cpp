#include "CSVReader.h"

using namespace pen;
using namespace std;
/**
 *
 *  Dir Manager Implementation
 *
 */

_CTOR_

DirManager::DirManager(const std::string& dir, const std::string& ext/* = ""*/)
    :_Dir(dir)
    ,_Extension(ext)
{}

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

const DirManager::FPaths DirManager::GetFilesWithExt() const
{
    //TODO change
    return _FilesPath;
}

_SETS_

void DirManager::SetDir(const std::string& value)
{
    _Dir = value;
}

void DirManager::SetExt(const std::string& value)
{
    _Extension = value;
}

void DirManager::Init()
{
    
}