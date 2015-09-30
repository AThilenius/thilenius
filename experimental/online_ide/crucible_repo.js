// Copyright 2015 Alec Thilenius
// All rights reserved.

var CrucibleRepo = function(crucible, repoProto) {
  // The ::crucible::Repo
  this.repoProto = repoProto;

  // private
  this.crucible = crucible;

  // private
  this.differencer = new Differencer();

  // private
  this.nextClId = 0;

  // The file tree at HEAD (in the format {relative_path : crucible::File, ...}
  this.headState = this.reconstructFilesForCL();
};

// TODO(athilenius): Add support for addFile and removeFile (see commit on how
// to do that)

CrucibleRepo.prototype.commit = function(relativePath, newSource) {
  var oldSource = this.headState[relativePath].source;
  // TODO(athilenius): Need to get the MD5 as well somehow
  var fileDelta = new FileDelta();
  fileDelta.file_info = new FileInfo();
  fileDelta.file_info.relative_path = relativePath;
  fileDelta.file_info.modify_timestamp = (new Date()).getTime().toString();
  fileDelta.file_info.md5 = "JS client, no md5";
  fileDelta.patches = this.differencer.patchesFromStrings(oldSource, newSource);
  var changeList = new ChangeList();
  changeList.change_list_uuid = "Pending CL: " + this.nextClId++;
  changeList.user_uuid = "Pending CL";
  changeList.timestamp = (new Date()).getTime().toString();
  changeList.modified_files = [fileDelta];
  changeList.added_files = [];
  changeList.removed_files = [];
  // We create a mock CL, and 'commit' it to the local repo, then update the
  // headState immediately. Once the commit comes back from Crucible, we modify
  // the mock commit.
  this.repoProto.change_lists.push(changeList);
  // Don't modify the repo_header latest_change_list_uuid or Crucible will think
  // we are behind head
  this.headState = this.reconstructFilesForCL();
  var that = this;
  this.crucible.enqueueCommit(
      this.repoProto.repo_header, changeList, function(commitedCl) {
        // Replace head repo uuid with commited ID
        that.repoProto.repo_header.latest_change_list_uuid =
            commitedCl.change_list_uuid;
        // Replace CL field with commited CL fields
        changeList.change_list_uuid = commitedCl.change_list_uuid;
        changeList.user_uuid = commitedCl.user_uuid;
        changeList.timestamp = commitedCl.timestamp;
      });
};

// private
CrucibleRepo.prototype.reconstructFilesForCL = function(changeListUuid) {
  var activeFiles = {};
  for (var i = 0; i < this.repoProto.change_lists.length; i++) {
    var changeList = this.repoProto.change_lists[i];
    for (var ai = 0; ai < changeList.added_files.length; ai++) {
      var addedFile = changeList.added_files[ai];
      // Make a copy of the file (so it doesn't modifty the original repo
      activeFiles[addedFile.file_info.relative_path] =
          jQuery.extend(true, {}, addedFile);
    }
    for (var mi = 0; mi < changeList.modified_files.length; mi++) {
      var fileDelta = changeList.modified_files[mi];
      var modifiedFile = activeFiles[fileDelta.file_info.relative_path];
      modifiedFile.source =
          this.differencer.applyPatches(modifiedFile.source, fileDelta.patches);
    }
    for (var ri = 0; ri < changeList.removed_files.length; ri++) {
      var fileInfo = changeList.removed_files[ri];
      delete activeFiles[fileInfo.relative_path];
    }
    if (changeListUuid && changeList.change_list_uuid == changeListUuid) {
      return activeFiles;
    }
  }
  return activeFiles;
};
