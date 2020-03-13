#ifndef _CSV_READER_H_
#define _CSV_READER_H_

#include "GDefines.h"
#include <string>
#include <vector>

#if _WINDOWS_
#include <windows.h>
#endif

namespace pen
{
	
	class DirManager
	{
		typedef std::vector<std::string> FPaths;
	public _CTOR_:

		DirManager(const std::string& dir, const std::string& ext = "");

	public _DTOR_:

		~DirManager();

	public _FUNC_:

		int GetTotalFilesCount() const;
		int GetExtFilesCount() const;

	public _GETS_:

		const std::string& GetDir() const;
		const FPaths& GetFilesPath() const;
		const std::string& GetExtension() const;
		const FPaths GetFilesWithExt() const;
		bool IsDirExists() const;

	public _SETS_:

		void SetDir(const std::string& value);
		void SetExt(const std::string& value);

	private _FUNC_:

		void Init();

	private _PROP_:
		std::string _Dir{};
		std::string _Extension{};
		bool _bDirExists{false};
		FPaths _FilesPath{};
	};



};

#endif // !_CSV_READER_H_

