//==============================================================================
//==  DIFFERENCER  =============================================================
//==============================================================================
namespace cpp differencer

enum DiffType {
  UNKNOWN,
  INSERT,
  DELETE,
  EQUAL
}

struct Diff {
  1:DiffType diff_type;
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
