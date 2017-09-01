'use strict';

var _ = require('underscore');
var crypto = require('crypto');
var fs = require('fs');
var path = require('path');
var s = require('underscore.string');

var URL_BASE = 'https://www.thilenius.com/verify';
var FROM = '"Thilenius" <noreply@thilenius.com>';
var SUBJECT = 'Thilenius Account Verification';

var secureToken = function() {
  return new Promise((res, rej) => {
    crypto.randomBytes(48, (err, buffer) => {
      res(buffer.toString('base64'));
    });
  });
};

module.exports = function(Person) {

  /**
   * The email template HTML. Will do a find-replace on:
   * '{{FULL_NAME}}'  -> User's Full Name
   * '{{TOKEN_HREF}}' -> The HREF for the verification URL
   */
  var verificationEmailTemplate = null;

  // Cache the verification email template.
  fs.readFile(path.join(__dirname, '../../server/views/verify.html'), 'utf8',
    (err, data) => {
      if (err) {
        throw err;
      }
      verificationEmailTemplate = data;
    });

  Person.afterRemote('create', function(context, person, next) {
    // Generate and save a crypo safe token
    secureToken()
      .then(token => {
        person.verificationToken = token;
        return person.save();
      })
      .then(person => {
        var url = URL_BASE + '?token=' + person.verificationToken + '&uid=' +
          person.id;
        var html = s.replaceAll(verificationEmailTemplate, '{{FULL_NAME}}',
          s.titleize(person.firstName + ' ' + person.lastName));
        html = s.replaceAll(html, '{{TOKEN_HREF}}', url);
        // Send email
        return Person.app.models.Email.send({
          to: person.email,
          from: FROM,
          subject: SUBJECT,
          html: html
        });
      })
      .then(res => next(), err => {
        console.log('Email send error: ', err);
        next(err);
      });
  });

  /**
   * Changing password requires that the old password be given and a
   * 'newPassword' field.
   */
  Person.observe('before save', (ctx, next) => {
    if (!ctx.isNewInstance && ctx.data && ctx.data.password) {
      if (!ctx.data.newPassword) {
        return next(new Error('A newPassword field must be specified.'));
      }
      Person.findById(ctx.currentInstance.id, (err, person) => {
        if (err) {
          return next(err);
        }
        // Check the old password
        person.hasPassword(ctx.data.password, (err, hasPassword) => {
          if (!hasPassword) {
            return next(new Error('Invalid Password'));
          }
          // Password is good, allow the update of password
          person.password = ctx.data.newPassword;
          person.save((err, person) => next(err ? err : null));
        });
      });
    } else {
      next();
    }
  });

  /**
   * Sets the role for the user. This route can only be used by an admin, and an
   * admin cannot demote another admin, but can promote someone to admin.
   */
  Person.prototype.addRoleMapping = function(roleName, next) {
    Person.app.models.Role.findOne({
      where: {
        name: roleName
      }
    }, (err, role) => {
      if (err) {
        return next(err);
      } else if (!role) {
        var error = new Error();
        error.statusCode = 404;
        error.message = 'Role Not Found';
        error.code = 'ROLE_NOT_FOUND';
        return next(error);
      }
      role.principals.create({
        principalType: Person.app.models.RoleMapping.USER,
        principalId: this.id
      }, (err, principle) => next(err ? err : null));
    });
  };

  Person.remoteMethod(
    'addRoleMapping', {
      accepts: [{
        arg: 'roleName',
        type: 'string',
        description: 'The name of the role to be set.',
        required: true
      }],
      isStatic: false
    });

  /**
   * Removes all instances (including duplicates) of the RoleMapping for the
   * role by name 'roleName' from the Person instance.
   */
  Person.prototype.removeRoleMapping = function(roleName, next) {
    Person.app.models.Role.findOne({
      where: {
        name: roleName
      }
    }, (err, role) => {
      if (err) {
        return next(err);
      } else if (!role) {
        var error = new Error();
        error.statusCode = 404;
        error.message = 'Role Not Found';
        error.code = 'ROLE_NOT_FOUND';
        return next(error);
      }
      Person.app.models.RoleMapping.destroyAll({
        principalType: Person.app.models.RoleMapping.USER,
        principalId: this.id,
        roleId: role.id
      }, (err, info) => next(err ? err : null, info));
    });
  };

  Person.remoteMethod(
    'removeRoleMapping', {
      accepts: [{
        arg: 'roleName',
        type: 'string',
        description: 'The name of the role to deleted.',
        required: true
      }],
      isStatic: false
    });

};
