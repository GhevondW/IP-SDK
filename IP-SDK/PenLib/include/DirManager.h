#ifndef _DIR_MANAGER_H_
#define _DIR_MANAGER_H_

#include "GDefines.h"
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace pen
{

	namespace fs = std::filesystem;

	class DirManager
	{
		typedef std::vector<std::string> FPaths;
		public _CTOR_:

		DirManager(const std::string& dir = "", const std::string& ext = "");

		public _DTOR_:

		~DirManager();

		public _FUNC_:

		int GetTotalFilesCount() const;
		int GetExtFilesCount() const;
		bool CheckFilePathInDir(const std::string& FileName = {}) const;
#if _DEBUG_
		void PrintClassInfo();
#endif // _DEBUG_
		public _GETS_:

		const std::string& GetDir() const;
		const FPaths& GetFilesPath() const;
		const std::string& GetExtension() const;
		const FPaths& GetFilesWithExt() const;
		bool IsDirExists() const;

		public _SETS_:

		void SetDir(const std::string& value);
		void SetExt(const std::string& value);

		private _FUNC_:

		void Init();
		void Clear();

		private _PROP_:
		std::string _Dir{};
		std::string _FilePath{};
		std::string _Extension{};
		bool _bDirExists{ false };
		FPaths _FilesPath{};
		FPaths _FilesPathNeeded{};
		fs::path _MainPath;
	};

};

#endif // !_DIR_MANAGER_H_
