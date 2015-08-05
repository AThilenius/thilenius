//==============================================================================
//==  AUTHENTICATION SERVICE  ==================================================
//==============================================================================
include "standard_exceptions.thrift"

enum PermissionLevel {
  INTERNAL = 1,
  SUPER = 2,
  ADMIN = 3,
  USER = 4,
  UNTRUSTED = 5
}

struct UserAtom {
  1: string uuid;
  2: PermissionLevel permission_level;

  // Only returned if queried with a ptoken
  3: optional string email_address;
}

exception EmailInvalidException {
  1: string message;
}

exception PasswordInvalidException {
  1: string message;
}

exception AuthenticationException {
  1: optional string failure_message,
  2: optional i32 error_code,
}

service Authentication {

  // Create a new user account
  void create_user (1:string email_address, 2:string password)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:EmailInvalidException email_invalid,
      3:PasswordInvalidException password_exception);

  UserAtom read_user (1:string ptoken)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:AuthenticationException authentication_exception);

  // Deleted the user of the give primary token.
  void delete_user (1:string ptoken)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:AuthenticationException authentication_exception);

  // Creates a primary token with the given email_address and password
  string create_ptoken (1:string email_address, 2:string password)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:AuthenticationException authentication_exception);

  // Creates a sercondary token fromt he given primary token
  string create_stoken (1:string ptoken)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:AuthenticationException authentication_exception);

}
