'use strict';

angular
  .module('app')
  .service('baseData', baseDataService);

/** @ngInject */
function baseDataService($rootScope, $q, Person, search) {

  // Shared
  this.role = null;
  this.person = null;
  this.ready = $q.defer();

  // Dealmaker
  this.dealmaker = {};

  // Client
  this.customers = null;

  /**
   * Watch for changes to currentUser promise.
   */
  $rootScope.$watch('currentUser', person => {
    if (!person) {
      // TODO: Flush cache?
      return;
    }
    person.$promise.then(person => {
      this.person = person;
      if (this.person.type === 'dealmaker') {
        this.loadDealmakerData_();
      } else if (this.person.type === 'user') {
        this.loadClientData_();
      }
    });
  });

  /**
   * Loads all data (and indexes search) for a Dealmaker's login.
   */
  this.loadDealmakerData_ = function() {
    Person.dealmakers({
      id: Person.getCurrentId(),
      filter: {
        include: [{
          customers: ['contacts']
        }]
      }
    }).$promise.then(dealmakers => {
      angular.extend(this.dealmaker, dealmakers[0]);
      // Process customers and contacts for search indexing
      var customers = this.dealmaker.customers.map(c =>
        angular.extend(c, {
          url: '/customer/' + c.id,
          icon: 'people'
        })
      );
      var contacts = _.flatten(customers.map(customer =>
        customer.contacts.map(contact =>
          angular.extend(contact, {
            url: '/contact/' + contact.id,
            icon: 'contacts',
            customer: customer
          })
        )
      ));
      search.setSearchGroup('Customers', customers, ['name']);
      search.setSearchGroup('Contacts', contacts, ['name', 'emailAddress',
        'fax', 'mobilePhone', 'title', 'workPhone'
      ]);
      this.ready.resolve();
    });
  };

  /**
   * Loads all data (and indes search) for a Client login.
   */
  this.loadClientData_ = function() {
    this.customers = Person.customers({
      id: Person.getCurrentId(),
      filter: {
        include: ['addresses', 'dealmaker', 'contacts', {
          deals: [{
            dealDetails: ['dealPrices', 'dealDetailVolume']
          }]
        }]
      }
    });
    this.customers.$promise.then(_ => {
      this.ready.resolve();
    });
  };

};
