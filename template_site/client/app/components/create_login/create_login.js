'use strict';

angular
  .module('app')
  .controller('createLoginController', createLoginController);

/** @ngInject */
function createLoginController($location, $scope, Person) {

  $scope.emailSent = false;
  $scope.error = null;

  /**
   * Creates a new Person, sending a verification email to them.
   */
  $scope.createPerson = function() {
    $scope.emailSent = true;
    $scope.error = null;
    Person.create({
      email: $scope.email.toLowerCase(),
      firstName: s.titleize($scope.firstName),
      lastName: s.titleize($scope.lastName),
      password: $scope.password
    }).$promise.then(res => $scope.emailGood = true, err => {
      $scope.email = '';
      $scope.error = err.data.error.message;
      $scope.emailSent = false;
    });
  };

};

/**
 * Passwords match directive
 */
angular
  .module('app')
  .directive('passwordCheck', passwordCheck);

/** @ngInject */
function passwordCheck() {
  return {
    require: 'ngModel',
    link: passwordCheckController
  };
}

/** @ngInject */
function passwordCheckController($scope, $element, $attrs, ctrl) {
  var firstPassword = '#' + $attrs.passwordCheck;
  $element.add(firstPassword).on('keyup', function() {
    $scope.$apply(() => {
      ctrl.$setValidity('pwmatch', $element.val() === $(firstPassword).val());
    });
  });
};
