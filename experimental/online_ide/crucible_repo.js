var CrucibleRepo = function(crucible, sentinel, client, repoHeader, readyHandler,
                            errorHandler) {
  // private
  this.crucible = crucible;

  // private
  this.sentinel = sentinel;

  // private
  this.client = client;

  // The ::crucible::RepoHeader
  this.repoHeader = repoHeader;

  // Called when the repo is ready and computed out to HEAD
  this.readyHandler = readyHandler;

  // Error callback (in the event of a Crucible error)
  this.errorHandler = errorHandler;

  // The ::crucible::Repo
  this.repo = null;

  // The file tree at HEAD (in the format {relative_path : crucible::File, ...}
  this.headState = null;

  // The latest stashed code (just ahead of HEAD, before a commit)
  this.latestStash = {
    relativePath : null,
    newSource : null
  };

  // Character count of diff JSON before a commit
  this.commitThreshold = 50;

  this.loadRepo();
};

// Saves new_source as the latest stash, diffs it against HEAD. If diff >
// threshold, commits repos
CrucibleRepo.prototype.stash = function(relativePath, newSource) {
  if (this.crucible.isCommitPending) {
    console.warn("Delaying stash because a commit is already in progress");
    return;
  }
  console.log("Stashing: " + relativePath);
  if (this.latestStash.relativePath &&
      this.latestStash.relativePath != relativePath) {
    this.commit();
  }
  this.latestStash.relativePath = relativePath;
  this.latestStash.newSource = newSource;
  // Diff it and check if it's above the commit threshold
  var dmp = new diff_match_patch();
  var patch = dmp.patch_make(this.headState[relativePath].source, newSource);
  if (patch.toString().length > this.commitThreshold) {
    console.log("Commiting file " + relativePath + " to repo " +
                this.repoHeader.repo_uuid);
    this.commit();
  }
};

// Commits any stashed change. Shouldn't need to call this manually except when
// a commit must be forced (like logoff)
CrucibleRepo.prototype.commit = function() {
  if (!this.latestStash.relativePath) {
    console.warn("cannot commit, no stashed changes");
    return;
  }
  if (this.crucible.isCommitPending) {
    console.warn("Delaying commit because one is already in progress");
    return;
  }
  this.crucible.isCommitPending = true;
  console.log("Commiting: " + this.latestStash.relativePath);
  console.log("From Text: " +
              this.headState[this.latestStash.relativePath].source + "\nTo: " +
              this.latestStash.newSource);
  var dmp = new diff_match_patch();
  var dmpPatches =
      dmp.patch_make(this.headState[this.latestStash.relativePath].source,
                     this.latestStash.newSource);
  var fileDelta =
      this.toCrucibleFileDelta(this.latestStash.relativePath, dmpPatches);
  // TODO(athilenius): Support adding/removing files
  var changeList = new ChangeList();
  changeList.modified_files = [fileDelta];
  var that = this;
  this.client.CommitAndDownstream(this.sentinel.token, this.repoHeader,
                                  changeList, null)
      .fail(function(jqXHR, status, error) {
        // Leave isCommitPending as true to keep it locked
        that.errorHandler(error);
        console.error("Crucible server error: " +
                      JSON.stringify(error, null, 2));
      })
      .done(function(result) {
        console.log("Commited repo CL: " + JSON.stringify(result, null, 2));
        that.repo.change_lists.push(result);
        that.repoHeader.latest_change_list_uuid = result.change_list_uuid;
        // Recompute head state
        that.headState = that.reconstructFilesForCL(
            that.repo, that.repoHeader.latest_change_list_uuid);
        that.crucible.isCommitPending = false;
      });
  this.latestStash = {
    relativePath : null,
    newSource : null
  };
};

// private
CrucibleRepo.prototype.loadRepo = function() {
  var that = this;
  this.client.GetRepoById(this.sentinel.token, this.repoHeader.repo_uuid, null)
      .fail(function(jqXHR, status, error) {
        that.errorHandler(error);
        console.error("Crucible server error: " +
                      JSON.stringify(error, null, 2));
      })
      .done(function(result) {
        console.log("Loaded repo: " + JSON.stringify(result, null, 2));
        that.repo = result;
        // Compute it out to head
        that.headState = that.reconstructFilesForCL(
            that.repo, that.repoHeader.latest_change_list_uuid);
        that.readyHandler(that);
      });
};

// private
CrucibleRepo.prototype.reconstructFilesForCL = function(repo, changeListUuid) {
  var activeFiles = {};
  for (var i = 0; i < repo.change_lists.length; i++) {
    var changeList = repo.change_lists[i];
    for (var j = 0; j < changeList.added_files.length; j++) {
      var file = changeList.added_files[j];
      activeFiles[file.file_info.relative_path] = file;
    }
    for (var j = 0; j < changeList.modified_files.length; j++) {
      var fileDelta = changeList.modified_files[j];
      var file = activeFiles[fileDelta.file_info.relative_path];
      file.source = this.applyPatches(file.source, fileDelta.patches);
    }
    for (var j = 0; j < changeList.removed_files.length; j++) {
      var fileInfo = changeList.removed_files[j];
      delete activeFiles[fileInfo.relative_path];
    }
  }
  return activeFiles;
};

// private
CrucibleRepo.prototype.applyPatches = function(originalText, patches) {
  var dmp = new diff_match_patch();
  // Need to convert ::differencer::Patch to diff_match_.patch_obj array
  var dmp_patches = [];
  for (var i = 0; i < patches.length; i++) {
    var patch = patches[i];
    var dmp_patch = new diff_match_patch.patch_obj();
    dmp_patch.start1 = patch.from_span.from;
    dmp_patch.length1 = patch.from_span.length;
    dmp_patch.start2 = patch.to_span.from;
    dmp_patch.length2 = patch.to_span.length;
    for (var j = 0; j < patch.diffs.length; j++) {
      var diff = patch.diffs[j];
      // [DIFF_EQUAL, 'jump'], [DIFF_DELETE, 's'], [DIFF_INSERT, 'ed']
      if (diff.diff_type == DIFF_TYPE_INSERT) {
        dmp_patch.diffs.push([ DIFF_INSERT, diff.text ]);
      } else if (diff.diff_type == DIFF_TYPE_DELETE) {
        dmp_patch.diffs.push([ DIFF_DELETE, diff.text ]);
      } else if (diff.diff_type == DIFF_TYPE_EQUAL) {
        dmp_patch.diffs.push([ DIFF_EQUAL, diff.text ]);
      }
    }
    dmp_patches.push(dmp_patch);
  }
  return dmp.patch_apply(dmp_patches, originalText)[0];
};

// private
CrucibleRepo.prototype.toCrucibleFileDelta = function(relativePath,
                                                      dmpPatches) {
  // TODO(athilenius): Need to get the MD5 as well somehow
  var fileInfo = new FileInfo();
  fileInfo.relative_path = relativePath;
  fileInfo.modify_timestamp = (new Date()).getTime().toString();
  fileInfo.md5 = "JS client, no md5";
  var fileDelta = new FileDelta();
  fileDelta.patches = [];
  fileDelta.file_info = fileInfo;
  fileDelta.file_type = FILE_TYPE_TEXT;
  for (var i = 0; i < dmpPatches.length; i++) {
    var dmpPatch = dmpPatches[i];
    var patch = new Patch();
    patch.diffs = [];
    patch.from_span = new Span();
    patch.from_span.from = dmpPatch.start1;
    patch.from_span.length = dmpPatch.length1;
    patch.to_span = new Span();
    patch.to_span.from = dmpPatch.start2;
    patch.to_span.length = dmpPatch.length2;
    for (var j = 0; j < dmpPatch.diffs.length; j++) {
      var dmpDiff = dmpPatch.diffs[j];
      var diff = new Diff();
      if (dmpDiff[0] == DIFF_INSERT) {
        diff.diff_type = DIFF_TYPE_INSERT;
      } else if (dmpDiff[0] == DIFF_DELETE) {
        diff.diff_type = DIFF_TYPE_DELETE;
      } else if (dmpDiff[0] == DIFF_EQUAL) {
        diff.diff_type = DIFF_TYPE_EQUAL;
      }
      diff.text = dmpDiff[1];
      patch.diffs.push(diff);
    }
    fileDelta.patches.push(patch);
  }
  return fileDelta;
};
