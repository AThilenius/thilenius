#===============================================================================
#===  AUTHENTICATION SERVICE  ==================================================
#===============================================================================
include "authentication.thrift"
include "standard_exceptions.thrift"

struct UserBody {
  1: string first_name;
  2: string last_name;
}

struct UserInfo {
  1: authentication.UserAtom user_atom;

  // Not returned if the token is secondary
  2: optional UserBody user_body;
}

exception DuplicateUserFoundException {
  1: authentication.UserAtom user_atom;
}

exception UserBodyNotFoundException {
  1: authentication.UserAtom user_atom;
}

service UserData {

  void create_user_data (1:string ptoken, 2:UserBody user_body)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:authentication.AuthenticationException authentication_exception);

  UserInfo read_user_data(1:string token)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:authentication.AuthenticationException authentication_exception,
      3:UserBodyNotFoundException user_body_not_found_exception);

  void update_user_data(1:string ptoken, 2:UserBody user_body)
    throws (
      1:standard_exceptions.InternalServerError internal_server_error,
      2:authentication.AuthenticationException authentication_exception);

}
