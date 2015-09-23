//==============================================================================
//==  DIFFERENCER  =============================================================
//==============================================================================
namespace cpp2 differencer.proto

const i32 DIFF_TYPE_UNKNOWN = 0;
const i32 DIFF_TYPE_INSERT = 1;
const i32 DIFF_TYPE_DELETE = 2;
const i32 DIFF_TYPE_EQUAL = 3;

struct Diff {
  1:i32 diff_type;
  2:string text;
}

struct Span {
  1:i32 from;
  2:i32 length;
}

struct Patch {
  1: Span from_span;
  2: Span to_span;
  3: list<Diff> diffs;
}
