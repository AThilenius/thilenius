/** Generated at 11/30/2015 21:39:52 */

/**
 *** Hardcoded Models ***
 */

using System;
using System.Collections.Generic;
using Newtonsoft.Json;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;
using RestSharp.Portable;
using LBXamarinSDK;
using LBXamarinSDK.LBRepo;
using System.Net.Http;
using System.Threading;
using System.Net;
using System.Text.RegularExpressions;
using Newtonsoft.Json.Converters;
using RestSharp.Portable.Deserializers;
using System.Diagnostics;
using System.Net.Sockets;

namespace LBXamarinSDK
{
	// Gateway: Communication with Server API
	public class Gateway
    {
        private static Uri BASE_URL = new Uri("http://10.0.0.1:3000/api/");
		private static RestClient _client = new RestClient {BaseUrl = BASE_URL};
        private static string _accessToken = null;
		private static bool _debugMode = false;
        private static CancellationTokenSource _cts = new CancellationTokenSource();
		private static int _timeout = 6000;
		private static bool initFlag = false;

		// Custom deserializer to handle timezones formats sent from loopback
		private class CustomConverter : IDeserializer
        {
            private static readonly JsonSerializerSettings SerializerSettings;
            static CustomConverter ()
            {
                SerializerSettings = new JsonSerializerSettings
                {
                    DateTimeZoneHandling = DateTimeZoneHandling.Local,
                    Converters = new List<JsonConverter> { new IsoDateTimeConverter() },
                    NullValueHandling = NullValueHandling.Ignore
                };
            }

            public T Deserialize<T>(IRestResponse response)
            {
                var type = typeof(T);
                var rawBytes = response.RawBytes;
                return (T)JsonConvert.DeserializeObject (UTF8Encoding.UTF8.GetString (rawBytes, 0, rawBytes.Length), type, SerializerSettings);
            }

            public System.Net.Http.Headers.MediaTypeHeaderValue ContentType { get; set; }
        }

		// Allow Console WriteLines to debug communication with server
		public static void SetDebugMode(bool isDebugMode)
		{
			_debugMode = isDebugMode;
			if(_debugMode)
			{
				Debug.WriteLine("******************************");
				Debug.WriteLine("** SDK Gateway Debug Mode.  **");
				Debug.WriteLine("******************************\n");
			}
		}

		// Sets the server URL to the local address
		public static void SetServerBaseURLToSelf()
        {
            var firstOrDefault = Dns.GetHostEntry(Dns.GetHostName()).AddressList.FirstOrDefault(ip => ip.AddressFamily == AddressFamily.InterNetwork);
            if (firstOrDefault != null)
            {
                string adrStr = "http://" + firstOrDefault.ToString() + ":3000/api/";
                if (_debugMode)
                    Debug.WriteLine("-------- >> DEBUG: Setting Gateway URL to " + adrStr);
                SetServerBaseURL(new Uri(adrStr));
            }
            else
            {
                if (_debugMode)
                    Debug.WriteLine("-------- >> DEBUG: Error finding self URL.");
                throw new Exception();
            }
        }

		// Debug mode getter
		public static bool GetDebugMode()
		{
			return _debugMode;
		}
		
		/*** Cancellation-Token methods, define a timeout for a server request ***/
		private static void ResetCancellationToken()
		{
			_cts = new CancellationTokenSource();
            _cts.CancelAfter(_timeout);
		}

        public static void SetTimeout(int timeoutMilliseconds = 6000)
        {
			_timeout = timeoutMilliseconds;
			ResetCancellationToken();
        }
		/* *** */

		// Define server Base Url for API requests. Example: "http://10.0.0.1:3000/api/"
        public static void SetServerBaseURL(Uri baseUrl)
        {
            BASE_URL = baseUrl;
            _client.BaseUrl = baseUrl;
        }

		// Sets an access token to be added as an authorization in all future server requests
        public static void SetAccessToken(AccessToken accessToken)
        {
            if (accessToken != null)
                _accessToken = accessToken.id;
        }

		// Get the access token ID currently being used by the gateway
		public static string GetAccessTokenId()
        {
            return _accessToken;
        }

		// Performs a request to determine if connected to server
        public static async Task<bool> isConnected(int timeoutMilliseconds = 6000)
		{
			SetTimeout(timeoutMilliseconds);
			_cts.Token.ThrowIfCancellationRequested();
			try
			{
				var request = new RestRequest ("/", new HttpMethod ("GET"));
				var response = await _client.Execute<JObject>(request, _cts.Token).ConfigureAwait(false);
				if (response != null)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			catch(Exception e)
			{
				if (_debugMode)
                    Debug.WriteLine("-------- >> DEBUG: Error: " + e.Message + " >>");	 
				return false;
			}
		}

		// Resets the authorization token
        public static void ResetAccessToken()
        {
            _accessToken = null;
        }
        
		// Makes a request through restSharp to server
		public static async Task<T> MakeRequest<T>(RestRequest request)
        {
            ResetCancellationToken();
            _cts.Token.ThrowIfCancellationRequested();
            _client.IgnoreResponseStatusCode = true;

            if (!initFlag)
            {
                _client.ReplaceHandler(typeof(JsonDeserializer), new CustomConverter());
                initFlag = true;
            }

            var response = await _client.Execute<JRaw>(request, _cts.Token).ConfigureAwait(false);
            var responseData = response.Data != null ? response.Data.ToString() : "";
            
            if (!response.IsSuccess)
            {
			
                if(_debugMode)
                    Debug.WriteLine("-------- >> DEBUG: Error performing request, status code " + (int)response.StatusCode + ", Payload: " + responseData);
                throw new RestException(responseData, (int)response.StatusCode);
            }

            return JsonConvert.DeserializeObject<T>(responseData);
        }


		// Parses a server request then makes it through MakeRequest
        public static async Task<T> PerformRequest<T>(string APIUrl, string json, string method = "POST", IDictionary<string, string> queryStrings = null)
		{
			RestRequest request = null;
            request = new RestRequest(APIUrl, new HttpMethod(method));

            if(_debugMode)
                Debug.WriteLine("-------- >> DEBUG: Performing " + method + " request at URL: '" + _client.BuildUri(request) + "', Json: " + (string.IsNullOrEmpty(json) ? "EMPTY" : json));

			// Add query parameters to the request
            if (queryStrings != null)
            {
                foreach (var query in queryStrings)
                {
                    if (!string.IsNullOrEmpty(query.Value))
                    {
                        request.AddParameter(query.Key, query.Value, ParameterType.QueryString);
                    }
                }
            }

			// Add authorization token to the request
            if (!String.IsNullOrEmpty(_accessToken))
            {
                request.AddHeader("Authorization", _accessToken);
            }

			// Add body parameters to the request
			if ((method == "POST" || method == "PUT") && json != "")
            {
				request.AddHeader("ContentType", "application/json");
				request.AddParameter ("application/json", JObject.Parse(json), ParameterType.RequestBody);
			}

			// Make the request, return response
			var response = await MakeRequest<T>(request).ConfigureAwait(false);
			return response;
		}

        // T is the expected return type, U is the input type. E.g. U is Car, T is Car
        public static async Task<T> PerformPostRequest<U, T>(U objToPost, string APIUrl, IDictionary<string, string> queryStrings = null)
        {
            var res = await PerformRequest<T>(APIUrl, JsonConvert.SerializeObject(objToPost), "POST", queryStrings).ConfigureAwait(false);
            return res;
        }

        // T is the expected return type. For example "Car" for get or "Car[]" for get all cars
        public static async Task<T> PerformGetRequest<T>(string APIUrl, IDictionary<string, string> queryStrings = null)
        {	
            var res = await PerformRequest<T>(APIUrl, "", "GET", queryStrings).ConfigureAwait(false);
            return res;
        }

        // T is the expected return type, U is the input type. E.g. U is Car, T is Car
        public static async Task<T> PerformPutRequest<U, T>(U objToPut, string APIUrl, IDictionary<string, string> queryStrings = null)
        {
            var res = await PerformRequest<T>(APIUrl, JsonConvert.SerializeObject(objToPut), "PUT", queryStrings).ConfigureAwait(false);
            return res;
        }
    }

	// Base model for all LBXamarinSDK Models
	public abstract class LBModel
    {
        public virtual String getID()
        {
            return "";
        }
    }

	// Allow conversion between the return type of login methods into AccessToken, e.g. "AccessToken myAccessToken = await Users.login(someCredentials);
	// TODO: Add this jobject->class implicit conversion as a templated function for all classes inheriting from model
	public partial class AccessToken : LBModel
    {
        public static implicit operator AccessToken(JObject jObj)
        {
            if (jObj == null)
            {
                return null;
            }
            return JsonConvert.DeserializeObject<AccessToken>(jObj.ToString());
        }
    }

	// Access Token model
	public partial class AccessToken : LBModel
    {
        [JsonProperty("id", NullValueHandling = NullValueHandling.Ignore)]
        public string id { get; set; }

        [JsonProperty("ttl", NullValueHandling = NullValueHandling.Ignore)]
        public long? _ttl { get; set; }
		[JsonIgnore]
		public long ttl
		{
			get { return _ttl ?? new long(); }
			set { _ttl = value; }
		}

        [JsonProperty("created", NullValueHandling = NullValueHandling.Ignore)]
        public DateTime? _created { get; set; }
		[JsonIgnore]
		public DateTime created
		{
			get { return _created ?? new DateTime(); }
			set { _created = value; }
		}


        [JsonProperty("userID", NullValueHandling = NullValueHandling.Ignore)]
        public string userID { get; set; }

		public override String getID()
        {
            return id;
        }
    }
	// GeoPoint primitive loopback type
	public class GeoPoint : LBModel
	{
		// Must be leq than 90: TODO: Add attributes or setter limitations
		[JsonProperty("lat", NullValueHandling = NullValueHandling.Ignore)]
		public double Latitude { get; set; }

		[JsonProperty("lng", NullValueHandling = NullValueHandling.Ignore)]
		public double Longitude { get; set; }
	}

	// Exception class, thrown on bad REST requests
	class RestException : Exception
    {
		public int StatusCode { get; private set; }

		private static int parseStatusCode(string responseString)
		{
            Regex statusCodeRegex = new Regex(@"[0-9]{3}");
            if (statusCodeRegex.IsMatch(responseString))
            {
                Match match = statusCodeRegex.Match(responseString);
				return Int32.Parse(match.Groups[0].Value);
			}
			else
			{
				return 0;
			}
		}

		public RestException(string responseString) : base(responseString)
		{
			StatusCode = parseStatusCode(responseString);
		}

		public RestException(string responseString, int StatusCode) : base(responseString)
		{
            this.StatusCode = StatusCode;
		}
    }
}
/**
 *** Dynamic Repositories ***
 */

namespace LBXamarinSDK
{
    namespace LBRepo
    {
		/* CRUD Interface holds the basic CRUD operations for all models.
		   In turn, all repositories will inherit from this.
		*/
        public abstract class CRUDInterface<T> where T : LBModel
        {
			private static readonly Dictionary<string, string> APIDictionary = new Dictionary<string, string>
            {
				{"person/create", "People"}, 
				{"person/upsert", "People"}, 
				{"person/exists", "People/:id/exists"}, 
				{"person/findbyid", "People/:id"}, 
				{"person/find", "People"}, 
				{"person/findone", "People/findOne"}, 
				{"person/updateall", "People/update"}, 
				{"person/deletebyid", "People/:id"}, 
				{"person/count", "People/count"}, 
				{"person/prototype$updateattributes", "People/:id"}, 
				{"club/create", "Clubs"}, 
				{"club/upsert", "Clubs"}, 
				{"club/exists", "Clubs/:id/exists"}, 
				{"club/findbyid", "Clubs/:id"}, 
				{"club/find", "Clubs"}, 
				{"club/findone", "Clubs/findOne"}, 
				{"club/updateall", "Clubs/update"}, 
				{"club/deletebyid", "Clubs/:id"}, 
				{"club/count", "Clubs/count"}, 
				{"club/prototype$updateattributes", "Clubs/:id"}, 
				{"world/create", "Worlds"}, 
				{"world/upsert", "Worlds"}, 
				{"world/exists", "Worlds/:id/exists"}, 
				{"world/findbyid", "Worlds/:id"}, 
				{"world/find", "Worlds"}, 
				{"world/findone", "Worlds/findOne"}, 
				{"world/updateall", "Worlds/update"}, 
				{"world/deletebyid", "Worlds/:id"}, 
				{"world/count", "Worlds/count"}, 
				{"world/prototype$updateattributes", "Worlds/:id"}, 
				{"server/create", "Servers"}, 
				{"server/exists", "Servers/:id/exists"}, 
				{"server/findbyid", "Servers/:id"}, 
				{"server/find", "Servers"}, 
				{"server/findone", "Servers/findOne"}, 
				{"server/deletebyid", "Servers/:id"}, 
				{"server/count", "Servers/count"}, 
				{"file/create", "Files"}, 
				{"file/upsert", "Files"}, 
				{"file/exists", "Files/:id/exists"}, 
				{"file/findbyid", "Files/:id"}, 
				{"file/find", "Files"}, 
				{"file/findone", "Files/findOne"}, 
				{"file/updateall", "Files/update"}, 
				{"file/deletebyid", "Files/:id"}, 
				{"file/count", "Files/count"}, 
				{"file/prototype$updateattributes", "Files/:id"}, 
			};

			// Getter for API paths of CRUD methods
			protected static String getAPIPath(String crudMethodName)
            {
				Type baseType = typeof(T);
				String dictionaryKey = string.Format("{0}/{1}", baseType.Name, crudMethodName).ToLower();

				if(!APIDictionary.ContainsKey(dictionaryKey))
				{
					if(Gateway.GetDebugMode())
						Debug.WriteLine("Error - no known CRUD path for " + dictionaryKey);
					throw new Exception();
				}
				return APIDictionary[dictionaryKey];
            }

            /* All the basic CRUD: Hardcoded */

			/*
			 * Create a new instance of the model and persist it into the data source
			 */
            public static async Task<T> Create(T theModel)
            {
                String APIPath = getAPIPath("Create");
                var response = await Gateway.PerformPostRequest<T, T>(theModel, APIPath).ConfigureAwait(false);
                return response;
            }

			/*
			 * Update an existing model instance or insert a new one into the data source
			 */
            public static async Task<T> Upsert(T theModel)
            {
                String APIPath = getAPIPath("Upsert");
                var response = await Gateway.PerformPutRequest<T, T>(theModel, APIPath).ConfigureAwait(false);
                return response;
            }

			/*
			 * Check whether a model instance exists in the data source
			 */
            public static async Task<bool> Exists(string ID)
            {
                String APIPath = getAPIPath("Exists");
                APIPath = APIPath.Replace(":id", ID);
                var response = await Gateway.PerformGetRequest<object>(APIPath).ConfigureAwait(false);
                return JObject.Parse(response.ToString()).First.First.ToObject<bool>();
            }

			/*
			 * Find a model instance by id from the data source
			 */
            public static async Task<T> FindById(String ID)
            {
                String APIPath = getAPIPath("FindById");
                APIPath = APIPath.Replace(":id", ID);
                var response = await Gateway.PerformGetRequest<T>(APIPath).ConfigureAwait(false);
                return response;
            }

			/*
			 * Find all instances of the model matched by filter from the data source
			 */
            public static async Task<IList<T>> Find(string filter = "")
            {
                String APIPath = getAPIPath("Find");
                IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				queryStrings.Add("filter", filter);
                var response = await Gateway.PerformGetRequest<T[]>(APIPath, queryStrings).ConfigureAwait(false);
                return response.ToList();
            }

			/*
			 * Find first instance of the model matched by filter from the data source
			 */
            public static async Task<T> FindOne(string filter = "")
            {
                String APIPath = getAPIPath("FindOne");
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				queryStrings.Add("filter", filter);
                var response = await Gateway.PerformGetRequest<T>(APIPath, queryStrings).ConfigureAwait(false);
                return response;
            }

			/*
			 * Update instances of the model matched by where from the data source
			 */
            public static async Task UpdateAll(T updateModel, string whereFilter)
            {
				String APIPath = getAPIPath("UpdateAll");
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				queryStrings.Add("where", whereFilter);
                var response = await Gateway.PerformPostRequest<T, string>(updateModel, APIPath, queryStrings).ConfigureAwait(false);
            }

			/*
			 * Delete a model instance by id from the data source
			 */
            public static async Task DeleteById(String ID)
            {
				String APIPath = getAPIPath("DeleteById");
                APIPath = APIPath.Replace(":id", ID);
                var response = await Gateway.PerformRequest<string>(APIPath, "", "DELETE").ConfigureAwait(false);
            }

			/*
			 * Count instances of the model matched by where from the data source
			 */
            public static async Task<int> Count(string whereFilter = "")
            {
                String APIPath = getAPIPath("Count");
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				queryStrings.Add("where", whereFilter);
                var response = await Gateway.PerformGetRequest<object>(APIPath, queryStrings).ConfigureAwait(false);
                return JObject.Parse(response.ToString()).First.First.ToObject<int>();
            }

			/*
			 * Update attributes for a model instance and persist it into the data source
			 */
            public static async Task<T> UpdateById(String ID, T update)
            {
                String APIPath = getAPIPath("prototype$updateAttributes");
                APIPath = APIPath.Replace(":id", ID);
                var response = await Gateway.PerformPutRequest<T, T>(update, APIPath).ConfigureAwait(false);
                return response;
            }
        }

		// Dynamic repositories for all Dynamic models:
		public class People : CRUDInterface<Person>
		{

			/*
			 * Find a related item by id for accessTokens.
			 */
			public static async Task<AccessToken> findByIdAccessTokens(string id, string fk)
			{
				string APIPath = "People/:id/accessTokens/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<AccessToken>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Delete a related item by id for accessTokens.
			 */
			public static async Task destroyByIdAccessTokens(string id, string fk)
			{
				string APIPath = "People/:id/accessTokens/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Update a related item by id for accessTokens.
			 */
			public static async Task<AccessToken> updateByIdAccessTokens(AccessToken data, string id, string fk)
			{
				string APIPath = "People/:id/accessTokens/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<AccessToken>(APIPath, bodyJSON, "PUT", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Find a related item by id for clubs.
			 */
			public static async Task<Club> findByIdClubs(string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Delete a related item by id for clubs.
			 */
			public static async Task destroyByIdClubs(string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Update a related item by id for clubs.
			 */
			public static async Task<Club> updateByIdClubs(Club data, string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "PUT", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Queries accessTokens of Person.
			 */
			public static async Task<IList<AccessToken>> getAccessTokens(string id, string filter = default(string))
			{
				string APIPath = "People/:id/accessTokens";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("filter", filter != null ? filter.ToString() : null);
				var response = await Gateway.PerformRequest<AccessToken[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Creates a new instance in accessTokens of this model.
			 */
			public static async Task<AccessToken> createAccessTokens(AccessToken data, string id)
			{
				string APIPath = "People/:id/accessTokens";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<AccessToken>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Deletes all accessTokens of this model.
			 */
			public static async Task deleteAccessTokens(string id)
			{
				string APIPath = "People/:id/accessTokens";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Counts accessTokens of Person.
			 */
			public static async Task<double> countAccessTokens(string id, string where = default(string))
			{
				string APIPath = "People/:id/accessTokens/count";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("where", where != null ? where.ToString() : null);
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<double>();
			}

			/*
			 * Queries clubs of Person.
			 */
			public static async Task<IList<Club>> getClubs(string id, string filter = default(string))
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("filter", filter != null ? filter.ToString() : null);
				var response = await Gateway.PerformRequest<Club[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Creates a new instance in clubs of this model.
			 */
			public static async Task<Club> createClubs(Club data, string id)
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Deletes all clubs of this model.
			 */
			public static async Task deleteClubs(string id)
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Counts clubs of Person.
			 */
			public static async Task<double> countClubs(string id, string where = default(string))
			{
				string APIPath = "People/:id/clubs/count";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("where", where != null ? where.ToString() : null);
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<double>();
			}

			/*
			 * Login a user with username/email and password.
			 */
			public static async Task<JObject> login(Person credentials, string include = default(string))
			{
				string APIPath = "People/login";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(credentials);
				queryStrings.Add("include", include != null ? include.ToString() : null);
				var response = await Gateway.PerformRequest<JObject>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Logout a user with access token.
			 */
			public static async Task logout()
			{
				string APIPath = "People/logout";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Confirm a user registration with email verification token.
			 */
			public static async Task confirm(string uid = default(string), string token = default(string), string redirect = default(string))
			{
				string APIPath = "People/confirm";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				queryStrings.Add("uid", uid != null ? uid.ToString() : null);
				queryStrings.Add("token", token != null ? token.ToString() : null);
				queryStrings.Add("redirect", redirect != null ? redirect.ToString() : null);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Reset password for a user with email.
			 */
			public static async Task resetPassword(Person options)
			{
				string APIPath = "People/reset";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(options);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Fetches belongsTo relation owner.
			 */
			public static async Task<Person> getForClub(string id, bool refresh = default(bool))
			{
				string APIPath = "Clubs/:id/owner";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("refresh", refresh != null ? refresh.ToString() : null);
				var response = await Gateway.PerformRequest<Person>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}
		}
		public class Clubs : CRUDInterface<Club>
		{

			/*
			 * Fetches belongsTo relation owner.
			 */
			public static async Task<Person> getOwner(string id, bool refresh = default(bool))
			{
				string APIPath = "Clubs/:id/owner";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("refresh", refresh != null ? refresh.ToString() : null);
				var response = await Gateway.PerformRequest<Person>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Find a related item by id for servers.
			 */
			public static async Task<Server> findByIdServers(string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Delete a related item by id for servers.
			 */
			public static async Task destroyByIdServers(string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Update a related item by id for servers.
			 */
			public static async Task<Server> updateByIdServers(Server data, string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "PUT", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Queries servers of Club.
			 */
			public static async Task<IList<Server>> getServers(string id, string filter = default(string))
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("filter", filter != null ? filter.ToString() : null);
				var response = await Gateway.PerformRequest<Server[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Creates a new instance in servers of this model.
			 */
			public static async Task<Server> createServers(Server data, string id)
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Deletes all servers of this model.
			 */
			public static async Task deleteServers(string id)
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Counts servers of Club.
			 */
			public static async Task<double> countServers(string id, string where = default(string))
			{
				string APIPath = "Clubs/:id/servers/count";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("where", where != null ? where.ToString() : null);
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<double>();
			}

			/*
			 * Find a related item by id for clubs.
			 */
			public static async Task<Club> findByIdForPerson(string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Delete a related item by id for clubs.
			 */
			public static async Task destroyByIdForPerson(string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Update a related item by id for clubs.
			 */
			public static async Task<Club> updateByIdForPerson(Club data, string id, string fk)
			{
				string APIPath = "People/:id/clubs/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "PUT", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Queries clubs of Person.
			 */
			public static async Task<IList<Club>> getForPerson(string id, string filter = default(string))
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("filter", filter != null ? filter.ToString() : null);
				var response = await Gateway.PerformRequest<Club[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Creates a new instance in clubs of this model.
			 */
			public static async Task<Club> createForPerson(Club data, string id)
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Deletes all clubs of this model.
			 */
			public static async Task deleteForPerson(string id)
			{
				string APIPath = "People/:id/clubs";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Counts clubs of Person.
			 */
			public static async Task<double> countForPerson(string id, string where = default(string))
			{
				string APIPath = "People/:id/clubs/count";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("where", where != null ? where.ToString() : null);
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<double>();
			}

			/*
			 * Fetches belongsTo relation club.
			 */
			public static async Task<Club> getForServer(string id, bool refresh = default(bool))
			{
				string APIPath = "Servers/:id/club";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("refresh", refresh != null ? refresh.ToString() : null);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}
		}
		public class Worlds : CRUDInterface<World>
		{
		}
		public class Servers : CRUDInterface<Server>
		{

			/*
			 * Fetches belongsTo relation club.
			 */
			public static async Task<Club> getClub(string id, bool refresh = default(bool))
			{
				string APIPath = "Servers/:id/club";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("refresh", refresh != null ? refresh.ToString() : null);
				var response = await Gateway.PerformRequest<Club>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Find a related item by id for servers.
			 */
			public static async Task<Server> findByIdForClub(string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Delete a related item by id for servers.
			 */
			public static async Task destroyByIdForClub(string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Update a related item by id for servers.
			 */
			public static async Task<Server> updateByIdForClub(Server data, string id, string fk)
			{
				string APIPath = "Clubs/:id/servers/:fk";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				APIPath = APIPath.Replace(":fk", (string)fk);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "PUT", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Queries servers of Club.
			 */
			public static async Task<IList<Server>> getForClub(string id, string filter = default(string))
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("filter", filter != null ? filter.ToString() : null);
				var response = await Gateway.PerformRequest<Server[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Creates a new instance in servers of this model.
			 */
			public static async Task<Server> createForClub(Server data, string id)
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(data);
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<Server>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * Deletes all servers of this model.
			 */
			public static async Task deleteForClub(string id)
			{
				string APIPath = "Clubs/:id/servers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * Counts servers of Club.
			 */
			public static async Task<double> countForClub(string id, string where = default(string))
			{
				string APIPath = "Clubs/:id/servers/count";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":id", (string)id);
				queryStrings.Add("where", where != null ? where.ToString() : null);
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<double>();
			}
		}
		public class Containers : CRUDInterface<Container>
		{

			/*
			 * No description given.
			 */
			public static async Task<IList<string>> getContainers()
			{
				string APIPath = "Containers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				var response = await Gateway.PerformRequest<string[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * No description given.
			 */
			public static async Task<JObject> createContainer(Container options)
			{
				string APIPath = "Containers";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				bodyJSON = JsonConvert.SerializeObject(options);
				var response = await Gateway.PerformRequest<JObject>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * No description given.
			 */
			public static async Task destroyContainer(string container)
			{
				string APIPath = "Containers/:container";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * No description given.
			 */
			public static async Task<JObject> getContainer(string container)
			{
				string APIPath = "Containers/:container";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				var response = await Gateway.PerformRequest<JObject>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * No description given.
			 */
			public static async Task<IList<string>> getFiles(string container)
			{
				string APIPath = "Containers/:container/files";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				var response = await Gateway.PerformRequest<string[]>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * No description given.
			 */
			public static async Task<JObject> getFile(string container, string file)
			{
				string APIPath = "Containers/:container/files/:file";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				APIPath = APIPath.Replace(":file", (string)file);
				var response = await Gateway.PerformRequest<JObject>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				return response;
			}

			/*
			 * No description given.
			 */
			public static async Task removeFile(string container, string file)
			{
				string APIPath = "Containers/:container/files/:file";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				APIPath = APIPath.Replace(":file", (string)file);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "DELETE", queryStrings).ConfigureAwait(false);
				
			}

			/*
			 * No description given.
			 */
			public static async Task<JObject> upload(string req, string res)
			{
				string APIPath = "Containers/:container/upload";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				var response = await Gateway.PerformRequest<object>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return JObject.Parse(response.ToString()).First.First.ToObject<JObject>();
			}

			/*
			 * No description given.
			 */
			public static async Task download(string container, string file, string res)
			{
				string APIPath = "Containers/:container/download/:file";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				APIPath = APIPath.Replace(":container", (string)container);
				APIPath = APIPath.Replace(":file", (string)file);
				var response = await Gateway.PerformRequest<string>(APIPath, bodyJSON, "GET", queryStrings).ConfigureAwait(false);
				
			}
		}
		public class Files : CRUDInterface<File>
		{

			/*
			 * Uploads a file
			 */
			public static async Task<JObject> upload(string ctx, string options = default(string))
			{
				string APIPath = "Files/upload";
				IDictionary<string, string> queryStrings = new Dictionary<string, string>();
				string bodyJSON = "";
				queryStrings.Add("options", options != null ? options.ToString() : null);
				var response = await Gateway.PerformRequest<JObject>(APIPath, bodyJSON, "POST", queryStrings).ConfigureAwait(false);
				return response;
			}
		}
		
	}
}

/**
 *** Dynamic Models ***
 */

namespace LBXamarinSDK
{
	public partial class Person : LBModel
	{
		[JsonProperty ("first_name", NullValueHandling = NullValueHandling.Ignore)]
		public String first_name { get; set; }

		[JsonProperty ("last_name", NullValueHandling = NullValueHandling.Ignore)]
		public String last_name { get; set; }

		[JsonProperty ("realm", NullValueHandling = NullValueHandling.Ignore)]
		public String realm { get; set; }

		[JsonProperty ("username", NullValueHandling = NullValueHandling.Ignore)]
		public String username { get; set; }

		[JsonProperty ("password", NullValueHandling = NullValueHandling.Ignore)]
		public String password { get; set; }

		[JsonProperty ("credentials", NullValueHandling = NullValueHandling.Ignore)]
		public Object credentials { get; set; }

		[JsonProperty ("challenges", NullValueHandling = NullValueHandling.Ignore)]
		public Object challenges { get; set; }

		[JsonProperty ("email", NullValueHandling = NullValueHandling.Ignore)]
		public String email { get; set; }

		[JsonIgnore]
		public bool emailVerified
		{
			get { return _emailVerified ?? new bool(); }
			set { _emailVerified = value; }
		}
		[JsonProperty ("emailVerified", NullValueHandling = NullValueHandling.Ignore)]
		private bool? _emailVerified { get; set; }

		[JsonProperty ("verificationToken", NullValueHandling = NullValueHandling.Ignore)]
		public String verificationToken { get; set; }

		[JsonProperty ("status", NullValueHandling = NullValueHandling.Ignore)]
		public String status { get; set; }

		[JsonIgnore]
		public DateTime created
		{
			get { return _created ?? new DateTime(); }
			set { _created = value; }
		}
		[JsonProperty ("created", NullValueHandling = NullValueHandling.Ignore)]
		private DateTime? _created { get; set; }

		[JsonIgnore]
		public DateTime lastUpdated
		{
			get { return _lastUpdated ?? new DateTime(); }
			set { _lastUpdated = value; }
		}
		[JsonProperty ("lastUpdated", NullValueHandling = NullValueHandling.Ignore)]
		private DateTime? _lastUpdated { get; set; }

		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}
	public partial class Club : LBModel
	{
		[JsonProperty ("name", NullValueHandling = NullValueHandling.Ignore)]
		public String name { get; set; }

		[JsonProperty ("members", NullValueHandling = NullValueHandling.Ignore)]
		public Object members { get; set; }

		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		[JsonProperty ("ownerId", NullValueHandling = NullValueHandling.Ignore)]
		public String ownerId { get; set; }

		[JsonProperty ("personId", NullValueHandling = NullValueHandling.Ignore)]
		public String personId { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}
	public partial class World : LBModel
	{
		[JsonProperty ("owner", NullValueHandling = NullValueHandling.Ignore)]
		public String owner { get; set; }

		[JsonProperty ("name", NullValueHandling = NullValueHandling.Ignore)]
		public String name { get; set; }

		[JsonProperty ("world_guid", NullValueHandling = NullValueHandling.Ignore)]
		public String world_guid { get; set; }

		[JsonIgnore]
		public double size
		{
			get { return _size ?? new double(); }
			set { _size = value; }
		}
		[JsonProperty ("size", NullValueHandling = NullValueHandling.Ignore)]
		private double? _size { get; set; }

		[JsonIgnore]
		public DateTime last_write_time
		{
			get { return _last_write_time ?? new DateTime(); }
			set { _last_write_time = value; }
		}
		[JsonProperty ("last_write_time", NullValueHandling = NullValueHandling.Ignore)]
		private DateTime? _last_write_time { get; set; }

		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}
	public partial class Server : LBModel
	{
		[JsonProperty ("state", NullValueHandling = NullValueHandling.Ignore)]
		public String state { get; set; }

		[JsonIgnore]
		public double ttl
		{
			get { return _ttl ?? new double(); }
			set { _ttl = value; }
		}
		[JsonProperty ("ttl", NullValueHandling = NullValueHandling.Ignore)]
		private double? _ttl { get; set; }

		[JsonIgnore]
		public DateTime created
		{
			get { return _created ?? new DateTime(); }
			set { _created = value; }
		}
		[JsonProperty ("created", NullValueHandling = NullValueHandling.Ignore)]
		private DateTime? _created { get; set; }

		[JsonProperty ("config", NullValueHandling = NullValueHandling.Ignore)]
		public String config { get; set; }

		[JsonProperty ("ip", NullValueHandling = NullValueHandling.Ignore)]
		public String ip { get; set; }

		[JsonIgnore]
		public double port
		{
			get { return _port ?? new double(); }
			set { _port = value; }
		}
		[JsonProperty ("port", NullValueHandling = NullValueHandling.Ignore)]
		private double? _port { get; set; }

		[JsonProperty ("container_uuid", NullValueHandling = NullValueHandling.Ignore)]
		public String container_uuid { get; set; }

		[JsonProperty ("worldId", NullValueHandling = NullValueHandling.Ignore)]
		public String worldId { get; set; }

		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		[JsonProperty ("clubId", NullValueHandling = NullValueHandling.Ignore)]
		public String clubId { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}
	public partial class Container : LBModel
	{
		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}
	public partial class File : LBModel
	{
		[JsonProperty ("name", NullValueHandling = NullValueHandling.Ignore)]
		public String name { get; set; }

		[JsonProperty ("type", NullValueHandling = NullValueHandling.Ignore)]
		public String type { get; set; }

		[JsonProperty ("url", NullValueHandling = NullValueHandling.Ignore)]
		public String url { get; set; }

		[JsonProperty ("id", NullValueHandling = NullValueHandling.Ignore)]
		public string id { get; set; }

		
		// This method identifies the ID field
		public override string getID()
		{
			return id;
		}
	}

	// Relationship classes:
	// None.
}
// Eof
