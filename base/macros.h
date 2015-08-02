// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef BASE_MACROS_H_
#define BASE_MACROS_H_

#define INIT_GOOGLE(argc, argv) \
  gflags::ParseCommandLineFlags(&argc, &argv, true); \
  google::InitGoogleLogging(argv[0]); \
  FLAGS_colorlogtostderr = true; \
  FLAGS_logtostderr = true; \
  testing::InitGoogleTest(&argc, argv); \
  testing::InitGoogleMock(&argc, argv);

#endif // BASE_MACROS_H_
