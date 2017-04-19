#include "rbfm.h"
#include "pfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) {
    PagedFileManager * temp = PagedFileManager::instance();                 //access the methods
    if (!temp->createFile(fileName)) {                                      //return createfile's value, successfully created the file
        fileStorage[fileName] = isClosed;
        return 0;
    }
    return -1;
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
    PagedFileManager * temp = PagedFileManager::instance();
    return temp->destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
    PagedFileManager * temp = PagedFileManager::instance();
    fileStorageItr = fileStorage.find(fileName);
    if (fileStorageItr == fileStorage.end()) {
        cout<< "file doesn't exist\n";
        return -1;
    }
    fileStorage[fileName] = isOpen;
    return temp->openFile(fileName, fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
    PagedFileManager * temp = PagedFileManager::instance();
    if (fileStorage[fileHandle.targetName] == isClosed) {
        return -1;
    }
            fileStorage[fileHandle.targetName] = isClosed;
            return temp->closeFile(fileHandle);
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {
    
    return -1;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    return -1;
}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
    return -1;
}
