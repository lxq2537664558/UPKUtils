#ifndef MODSCRIPT_H
#define MODSCRIPT_H

#include <map>
#include <sstream>

#include "ModParser.h"
#include "UPKUtils.h"

enum class UPKScope
{
    Package = 0,
    Name = 1,
    Import = 2,
    Export = 3,
    Object = 4
};
std::string FormatUPKScope(UPKScope scope);

class ModScript
{
public:
    ModScript(): UPKPath("") { InitStreams(); SetBad(); }
    ~ModScript() {};
    ModScript(const char* filename): UPKPath("") { InitStreams(); Parse(filename); }
    ModScript(const char* filename, const char* pathname) { InitStreams(); Parse(filename); SetUPKPath(pathname); }
    /// Init stream objects
    void InitStreams(std::ostream& err = std::cerr, std::ostream& res = std::cout);
    /// parse mod file to build execution stack
    bool Parse(const char* filename);
    /// set path to upk files, referenced inside mod files
    void SetUPKPath(const char* pathname) { UPKPath = pathname; }
    /// execute script
    bool ExecuteStack();
    /// state
    //std::string GetErrors() { return ErrorMessages.str(); }
    //std::string GetResults() { return ExecutionResults.str(); }
    //std::string GetBackupScript() { return BackupScript.str(); }
    std::string GetBackupScript();
    bool IsGood() { return ScriptState.Good; }
protected:
    typedef bool (ModScript::*ExecFunction)(const std::string&);
    struct ScriptCommand
    {
        std::string Name;
        std::string Param;
        ExecFunction Exec;
    };
    ModParser Parser;
    std::string UPKPath;
    std::map<std::string, ExecFunction> Executors;
    std::vector<ScriptCommand> ExecutionStack;
    std::ostream *ErrorMessages;
    std::ostream *ExecutionResults;
    std::map<std::string, std::string> BackupScript;
    std::multimap<std::string, std::string> GUIDs;
    std::vector<std::string> UPKNames;
    std::map<std::string, std::string> Alias;
    void SetExecutors(); /// map names to keys/sections and functions
    struct
    {
        std::string UPKName;
        UPKUtils Package;
        UPKScope Scope;
        uint32_t ObjIdx;
        size_t Offset;
        size_t RelOffset;
        size_t MaxOffset;
        std::string Behaviour;
        bool Good;
    } ScriptState;
    void ResetScope() { ScriptState.Scope = UPKScope::Package; ScriptState.ObjIdx = 0; ScriptState.Offset = 0; ScriptState.RelOffset = 0; ScriptState.MaxOffset = 0; ScriptState.Behaviour = "KEEP"; }
    bool SetBad() { return (ScriptState.Good = false); }
    bool SetGood() { return (ScriptState.Good = true); }
    void AddUPKName(std::string upkname);
    /// methods to implement mod file commands
    bool FormatModName(const std::string& Param);
    bool FormatAuthor(const std::string& Param);
    bool FormatDescription(const std::string& Param);
    bool OpenPackage(const std::string& Param);
    bool SetGUID(const std::string& Param);
    bool SetGlobalOffset(const std::string& Param);
    bool SetObject(const std::string& Param);
    bool SetNameEntry(const std::string& Param);
    bool SetImportEntry(const std::string& Param);
    bool SetExportEntry(const std::string& Param);
    bool SetRelOffset(const std::string& Param);
    bool WriteModdedHEX(const std::string& Param);
    bool WriteModdedCode(const std::string& Param);
    bool WriteReplacementCode(const std::string& Param);
    bool WriteInsertCode(const std::string& Param);
    bool WriteUndoMoveResize(const std::string& Param);
    bool WriteModdedFile(const std::string& Param);
    bool WriteByteValue(const std::string& Param);
    bool WriteFloatValue(const std::string& Param);
    bool WriteIntValue(const std::string& Param);
    bool WriteUnsignedValue(const std::string& Param);
    bool WriteRename(const std::string& Param);
    bool SetDataChunkOffset(const std::string& Param);
    bool SetCodeOffset(const std::string& Param);
    bool WriteMoveExpandLegacy(const std::string& Param);
    bool WriteNameIdx(const std::string& Param);
    bool WriteObjectIdx(const std::string& Param);
    bool AddAlias(const std::string& Param);
    /// end-of-block indicators are just skipped, as they don't actually used
    bool Sink(const std::string& Param);
    /// helpers
    bool CheckMoveResize(size_t DataSize);
    bool MoveResizeAtRelOffset(int ObjSize);
    bool WriteBinaryData(const std::vector<char>& DataChunk);
    bool IsInsideScope(size_t DataSize = 1);
    size_t GetDiff(size_t DataSize);
    /// parse script
    std::string ParseScript(std::string ScriptData, unsigned* ScriptMemSizeRef = nullptr);
    bool IsHEX(std::string word);
    bool IsToken(std::string word);
    bool IsCommand(std::string word);
    std::string TokenToHEX(std::string Token, unsigned* MemSizeRef = nullptr);
};

#endif // MODSCRIPT_H
