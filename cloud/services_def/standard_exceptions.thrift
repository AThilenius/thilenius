#===============================================================================
#===  STANDARD EXCEPTIONS  =====================================================
#===============================================================================

exception InternalServerError {
  1:optional string failure_message;
}

exception InsificientPermissions {
  2:optional string failure_message;
}
