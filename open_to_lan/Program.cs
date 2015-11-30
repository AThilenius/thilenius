using System;
using System.Collections.Generic;
using LBXamarinSDK;
using LBXamarinSDK.LBRepo;
using System.IO;
using Amazon.S3;
using Amazon.S3.Transfer;
using Ionic.Zip;
using System.Threading.Tasks;

namespace TestCShartApp {
  class UploadFileMPUHighLevelAPI {
    const string BUCKET_NAME = "opentolan";

    class LocalWorld {
      public String Path;
    }

    public static long DirSize(DirectoryInfo d) {
      long Size = 0;
      // Add file sizes.
      FileInfo[] fis = d.GetFiles();
      foreach (FileInfo fi in fis) {
        Size += fi.Length;
      }
      // Add subdirectory sizes.
      DirectoryInfo[] dis = d.GetDirectories();
      foreach (DirectoryInfo di in dis) {
        Size += DirSize(di);
      }
      return(Size);
    }

    static void Main(string[] args) {
      String username = "athilenius";
      Gateway.SetDebugMode(true);
      Gateway.SetServerBaseURL(new Uri("http://192.168.59.103:3000/api/"));
      Task<IList<World>> worldsTask = Worlds.Find("{\"where\":{\"owner\":\"athilenius\"}}");
      worldsTask.Wait();
      IList<World> worlds = worldsTask.Result;
      Console.WriteLine("Uploaded Worlds:");
      Console.WriteLine("{0,-20}{1,-10}{2,-15}", "Name", "Size", "Last Play Date");
      foreach (World world in worlds) {
//        Console.WriteLine("{0,-10}{1,-20}{2,-10}{3,-15}",
//          world.name,
//          world.size,
//          world.last_write_time);
        Console.WriteLine(world.name);
      }
      Console.WriteLine();
      String minecraftDir = null;
      if (Environment.OSVersion.Platform == PlatformID.MacOSX ||
          Environment.OSVersion.Platform == PlatformID.Unix) {
        minecraftDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Personal), "Library", "Application Support", "minecraft", "saves");
      } else if (Environment.OSVersion.Platform == PlatformID.Win32NT ||
                 Environment.OSVersion.Platform == PlatformID.Win32S ||
                 Environment.OSVersion.Platform == PlatformID.Win32Windows) {
        minecraftDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), ".minecraft", "saves");
      }
      Console.WriteLine("Local Worlds:");
      Console.WriteLine("{0,-10}{1,-20}{2,-10}{3,-15}", "Number", "Name", "Size", "Last Play Date");
      String[] worldDirs = Directory.GetDirectories(minecraftDir);
      for (int i = 0; i < worldDirs.Length; i++) {
        String directory = worldDirs[i];
        DirectoryInfo directoryInfo = new DirectoryInfo(directory);
        Console.WriteLine("{0,-10}{1,-20}{2,-10}{3,-15}",
          i,
          directoryInfo.Name,
          (DirSize(directoryInfo) / 1000000.0f).ToString("F1") + " MB",
          directoryInfo.LastWriteTime.Date);
      }
      Console.Write("What world do you want to upload? ");
      int worldNumber = -1;
      while (!int.TryParse(Console.ReadLine(), out worldNumber) || worldNumber < 0 ||
             worldNumber >= worldDirs.Length) {
        Console.Write("What world do you want to upload? ");
      }
      DirectoryInfo dirInfo = new DirectoryInfo(worldDirs[worldNumber]);
      Console.WriteLine("Zipping world " + dirInfo.Name);
      String zipGuid = Guid.NewGuid().ToString();
      String fullZipPath = Path.Combine(Path.GetTempPath(), "otl", zipGuid + ".zip");
      Directory.CreateDirectory(Path.GetDirectoryName(fullZipPath));
      ZipFile zip = new ZipFile(fullZipPath);
      zip.AddDirectory(dirInfo.FullName);
      zip.Save();
      Console.WriteLine("Creating S3 Object: " + zipGuid);
      // Upload it to S3
      try {
        TransferUtility fileTransferUtility = new TransferUtility(new AmazonS3Client(Amazon.RegionEndpoint.USWest1));
        TransferUtilityUploadRequest request = new TransferUtilityUploadRequest {
          BucketName = BUCKET_NAME,
          FilePath = fullZipPath
        };
        request.UploadProgressEvent
            += new EventHandler<UploadProgressArgs>(uploadRequest_UploadPartProgressEvent);
        fileTransferUtility.Upload(request);
        Console.WriteLine();
        Console.WriteLine("World uploaded successfuly!");
      } catch (AmazonS3Exception e) {
        Console.WriteLine(e.Message, e.InnerException);
      }
      // Delete the temp
      System.IO.File.Delete(fullZipPath);
//      Task<World> worldCreateTask = Worlds.Create(new World {
//        owner = 
//        name = dirInfo.Name,
//        world_guid = zipGuid
//      });
//      worldCreateTask.Wait();
    }

    static void uploadRequest_UploadPartProgressEvent(
      object sender, UploadProgressArgs e) {
      Console.Write("\rUploading: [");
      for (int i = 0; i < e.PercentDone / 2; i++) {
        Console.Write("=");
      }
      for (int i = 0; i < 50 - (e.PercentDone / 2); i++) {
        Console.Write(" ");
      }
      Console.Write("]");
    }

  }
}