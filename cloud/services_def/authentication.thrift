// Authentication Service

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

exception TokenExpiredException { }

enum PermissionLevel {
  SUPER = 1,
  ADMIN = 2,
  USER = 3,
  SECONDARY = 4,
  UNTRUSTED = 5
}

service Authentication {

  // Create a new user account
  void create_user (1:string email_address, 2:string password)
    throws (
      1:EmailInvalidException email_invalid,
      2:PasswordInvalidException password_exception);

  // Returns the permission level of the given token. A p_token will return the
  // permission of the user it belongs to and a s_token will always return
  // SECONDARY if it is valid
  PermissionLevel read_user (1:string ptoken)
    throws (1:AuthenticationException authentication_exception);

  // Deleted the user of the give primary token.
  void delete_user (1:string ptoken)
    throws (1:AuthenticationException authentication_exception);

  // Creates a primary token with the given email_address and password
  string create_ptoken (1:string email_address, 2:string password)
    throws (1:AuthenticationException authentication_exception);

  // Creates a sercondary token fromt he given primary token
  string create_stoken (1:string ptoken)
    throws (1:AuthenticationException authentication_exception);

}
