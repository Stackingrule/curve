/*
  * Copyright 2010-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
  *
  * Licensed under the Apache License, Version 2.0 (the "License").
  * You may not use this file except in compliance with the License.
  * A copy of the License is located at
  *
  *  http://aws.amazon.com/apache2.0
  *
  * or in the "license" file accompanying this file. This file is distributed
  * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
  * express or implied. See the License for the specific language governing
  * permissions and limitations under the License.
  */

#pragma once

#include <aws/core/Core_EXPORTS.h>

#include <aws/core/http/URI.h>
#include <aws/core/http/HttpTypes.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSStreamFwd.h>
#include <aws/core/utils/stream/ResponseStream.h>
#include <aws/core/monitoring/HttpClientMetrics.h>
#include <memory>
#include <functional>

using namespace Aws::Monitoring;

namespace Aws
{
    namespace Http
    {
        extern AWS_CORE_API const char* DATE_HEADER;
        extern AWS_CORE_API const char* AWS_DATE_HEADER;
        extern AWS_CORE_API const char* AWS_SECURITY_TOKEN;
        extern AWS_CORE_API const char* ACCEPT_HEADER;
        extern AWS_CORE_API const char* ACCEPT_CHAR_SET_HEADER;
        extern AWS_CORE_API const char* ACCEPT_ENCODING_HEADER;
        extern AWS_CORE_API const char* AUTHORIZATION_HEADER;
        extern AWS_CORE_API const char* AWS_AUTHORIZATION_HEADER;
        extern AWS_CORE_API const char* COOKIE_HEADER;
        extern AWS_CORE_API const char* CONTENT_LENGTH_HEADER;
        extern AWS_CORE_API const char* CONTENT_TYPE_HEADER;
        extern AWS_CORE_API const char* USER_AGENT_HEADER;
        extern AWS_CORE_API const char* VIA_HEADER;
        extern AWS_CORE_API const char* HOST_HEADER;
        extern AWS_CORE_API const char* AMZ_TARGET_HEADER;
        extern AWS_CORE_API const char* X_AMZ_EXPIRES_HEADER;
        extern AWS_CORE_API const char* CONTENT_MD5_HEADER;
        extern AWS_CORE_API const char* API_VERSION_HEADER;

        class HttpRequest;
        class HttpResponse;

        /**
         * closure type for recieving notifications that data has been recieved.
         */
        typedef std::function<void(const HttpRequest*, HttpResponse*, long long)> DataReceivedEventHandler;
        /**
         * closure type for receiving notifications that data has been sent.
         */
        typedef std::function<void(const HttpRequest*, long long)> DataSentEventHandler;
        /**
         * Closure type for handling whether or not a request should be canceled.
         */
        typedef std::function<bool(const HttpRequest*)> ContinueRequestHandler;

        /**
          * Abstract class for representing an HttpRequest.
          */
        class AWS_CORE_API HttpRequest
        {
        public:
            /**
             * Initializes an HttpRequest object with uri and http method.
             */
            HttpRequest(const URI& uri, HttpMethod method) :
                m_uri(uri), m_method(method)
            {}

            virtual ~HttpRequest() {}

            /**
             * Get All headers for this request.
             */
            virtual HeaderValueCollection GetHeaders() const = 0;
            /**
             * Get the value for a Header based on its name. (in default StandardHttpRequest implementation, an empty string will be returned if headerName dosen't exist)
             */
            virtual const Aws::String& GetHeaderValue(const char* headerName) const = 0;
            /**
             * Add a header pair
             */
            virtual void SetHeaderValue(const char* headerName, const Aws::String& headerValue) = 0;
            /**
             * Creates a shared_ptr of HttpRequest with uri, method, and closure for how to create a response stream.
             */
            virtual void SetHeaderValue(const Aws::String& headerName, const Aws::String& headerValue) = 0;
            /**
             * Deletes a header from the request by name.
             */
            virtual void DeleteHeader(const char* headerName) = 0;
            /**
             * Adds a content body stream to the request. This stream will be used to send the body to the endpoint.
             */
            virtual void AddContentBody(const std::shared_ptr<Aws::IOStream>& strContent) = 0;
            /**
             * Gets the content body stream that will be used for this request.
             */
            virtual const std::shared_ptr<Aws::IOStream>& GetContentBody() const = 0;
            /**
             * Returns true if a header exists in the request with name
             */
            virtual bool HasHeader(const char* name) const = 0;
            /**
             * Get size in bytes of the request when as it will be going accross the wire.
             */
            virtual int64_t GetSize() const = 0;
            /**
             * Gets the factory for creating the stream that will be used in the http response.
             */
            virtual const Aws::IOStreamFactory& GetResponseStreamFactory() const = 0;
            /**
            * Sets the factory for creating the stream that will be used in the http response.
            */
            virtual void SetResponseStreamFactory(const Aws::IOStreamFactory& factory) = 0;
            /**
             * Gets underlying URI object with mutation access.
             */
            inline URI& GetUri()
            {
                return m_uri;
            }
            /**
             * Gets the underlying URI object.
             */
            const URI& GetUri() const { return m_uri; }
            /**
             * Converts the URI into a string and returns it. If includeQueryString is set to true, the query string
             * will be included in the returned value. 
             */
            inline Aws::String GetURIString(bool includeQueryString = true) const
            {
                return m_uri.GetURIString(includeQueryString);
            }
            /**
             * Get the http method for this request.
             */
            inline HttpMethod GetMethod() const
            {
                return m_method;
            }
            /**
             * Gets the query string from the URI on this request.
             */
            inline const Aws::String& GetQueryString() const
            {
                return m_uri.GetQueryString();
            }
            /**
             * Normalizes the URI for use with signing.
             */
            inline void CanonicalizeRequest()
            {
                m_uri.CanonicalizeQueryString();
            }
            /**
             * Gets the query string for the underlying URI as a key value mapping
             */
            inline QueryStringParameterCollection GetQueryStringParameters() const
            {
                return m_uri.GetQueryStringParameters();
            }
            /**
             * Adds a query string parameter to the underlying URI by key and value.
             */
            inline void AddQueryStringParameter(const char* key, const Aws::String& value)
            {
                m_uri.AddQueryStringParameter(key, value);
            }

            inline bool HasDate() const 
            {
                return HasHeader(DATE_HEADER);
            }
            /**
             * Gets date header.
             */
            inline const Aws::String& GetDate() const
            {
                return GetHeaderValue(DATE_HEADER);
            }
            /**
            * Gets date header.
            */
            inline void SetDate(const Aws::String& value)
            {
                SetHeaderValue(DATE_HEADER, value);
            }

            inline bool HasAccept() const
            {
                return HasHeader(ACCEPT_HEADER);
            }
            /**
             * Gets accept header.
             */
            inline const Aws::String& GetAccept() const
            {
                return GetHeaderValue(ACCEPT_HEADER);
            }
            /**
             * Gets accept header.
             */
            inline void SetAccept(const Aws::String& value)
            {
                SetHeaderValue(ACCEPT_HEADER, value);
            }

            inline bool HasAcceptCharSet() const
            {
                return HasHeader(ACCEPT_CHAR_SET_HEADER);
            }
            /**
             * Gets Accept CharSet header.
             */
            inline const Aws::String& GetAcceptCharSet() const
            {
                return GetHeaderValue(ACCEPT_CHAR_SET_HEADER);
            }
            /**
             * Sets Accept CharSet header.
             */
            inline void SetAcceptCharSet(const Aws::String& value)
            {
                SetHeaderValue(ACCEPT_CHAR_SET_HEADER, value);
            }

            inline bool HasAcceptEncoding() const
            {
                return HasHeader(ACCEPT_ENCODING_HEADER);
            }
            /**
             * Gets accept encoding header.
             */
            inline const Aws::String& GetAcceptEncoding() const
            {
                return GetHeaderValue(ACCEPT_ENCODING_HEADER);
            }
            /**
             * Sets accept encoding header.
             */
            inline void SetAcceptEncoding(const Aws::String& value)
            {
                SetHeaderValue(ACCEPT_ENCODING_HEADER, value);
            }

            inline bool HasAuthorization() const
            {
                return HasHeader(AUTHORIZATION_HEADER);
            }
            /**
             * Gets authorization encoding header.
             */
            inline const Aws::String& GetAuthorization() const
            {
                return GetHeaderValue(AUTHORIZATION_HEADER);
            }
            /**
             * Set authorization header.
             */
            inline void SetAuthorization(const Aws::String& value)
            {
                SetHeaderValue(AUTHORIZATION_HEADER, value);
            }

            inline bool HasAwsAuthorization() const
            {
                return HasHeader(AWS_AUTHORIZATION_HEADER);
            }
            /**
             * Gets aws authorization header.
             */
            inline const Aws::String& GetAwsAuthorization() const
            {
                return GetHeaderValue(AWS_AUTHORIZATION_HEADER);
            }
            /**
            * Sets aws authorization header.
            */
            inline void SetAwsAuthorization(const Aws::String& value)
            {
                SetHeaderValue(AWS_AUTHORIZATION_HEADER, value);
            }

            inline bool HasAwsSessionToken() const
            {
                return HasHeader(AWS_SECURITY_TOKEN);
            }
            /**
            * Gets session token header.
            */
            inline const Aws::String& GetAwsSessionToken() const
            {
                return GetHeaderValue(AWS_SECURITY_TOKEN);
            }
            /**
            * Sets session token header.
            */
            inline void SetAwsSessionToken(const Aws::String& value)
            {
                SetHeaderValue(AWS_SECURITY_TOKEN, value);
            }

            inline bool HasCookie() const
            {
                return HasHeader(COOKIE_HEADER);
            }
            /**
            * Gets cookie header.
            */
            inline const Aws::String& GetCookie() const
            {
                return GetHeaderValue(COOKIE_HEADER);
            }
            /**
            * Sets cookie header.
            */
            inline void SetCookie(const Aws::String& value)
            {
                SetHeaderValue(COOKIE_HEADER, value);
            }

            inline bool HasContentLength() const
            {
                return HasHeader(CONTENT_LENGTH_HEADER);
            }
            /**
            * Gets content-length header.
            */
            inline const Aws::String& GetContentLength() const
            {
                return GetHeaderValue(CONTENT_LENGTH_HEADER);
            }
            /**
            * Sets content-length header.
            */
            inline void SetContentLength(const Aws::String& value)
            {
                SetHeaderValue(CONTENT_LENGTH_HEADER, value);
            }

            inline bool HasContentType() const
            {
                return HasHeader(CONTENT_TYPE_HEADER);
            }
            /**
            * Gets content-type header.
            */
            inline const Aws::String& GetContentType() const
            {
                return GetHeaderValue(CONTENT_TYPE_HEADER);
            }
            /**
            * sets content-type header.
            */
            inline void SetContentType(const Aws::String& value)
            {
                SetHeaderValue(CONTENT_TYPE_HEADER, value);
            }

            inline bool HasUserAgent() const
            {
                return HasHeader(USER_AGENT_HEADER);
            }
            /**
            * Gets User Agent header.
            */
            inline const Aws::String& GetUserAgent() const
            {
                return GetHeaderValue(USER_AGENT_HEADER);
            }
            /**
            * Sets User Agent header.
            */
            inline void SetUserAgent(const Aws::String& value)
            {
                SetHeaderValue(USER_AGENT_HEADER, value);
            }

            inline bool HasVia() const
            {
                return HasHeader(VIA_HEADER);
            }
            /**
            * Gets via header header.
            */
            inline const Aws::String& GetVia() const
            {
                return GetHeaderValue(VIA_HEADER);
            }
            /**
             * Sets via header.
             */
            inline void SetVia(const Aws::String& value)
            {
                SetHeaderValue(VIA_HEADER, value);
            }

            /**
             * Has Api version header x-amz-api-version
             */
            inline bool HasApiVersion() const
            {
                return HasHeader(API_VERSION_HEADER);
            }

            /**
            * Gets Api version header x-amz-api-version.
            */
            inline const Aws::String& GetApiVersion() const
            {
                return GetHeaderValue(API_VERSION_HEADER);
            }
            /**
             * Sets Api version header x-amz-api-version.
             */
            inline void SetApiVersion(const Aws::String& value)
            {
                SetHeaderValue(API_VERSION_HEADER, value);
            }

            /**
             * Sets the closure for receiving events when data is received from the server.
             */
            inline void SetDataReceivedEventHandler(const DataReceivedEventHandler& dataReceivedEventHandler) { m_onDataReceived = dataReceivedEventHandler; }
            /**
             * Sets the closure for receiving events when data is received from the server.
             */
            inline void SetDataReceivedEventHandler(DataReceivedEventHandler&& dataReceivedEventHandler) { m_onDataReceived = std::move(dataReceivedEventHandler); }
            /**
             * Sets the closure for receiving events when data is sent to the server.
             */
            inline void SetDataSentEventHandler(const DataSentEventHandler& dataSentEventHandler) { m_onDataSent = dataSentEventHandler; }
            /**
             * Sets the closure for receiving events when data is sent to the server.
             */
            inline void SetDataSentEventHandler(DataSentEventHandler&& dataSentEventHandler) { m_onDataSent = std::move(dataSentEventHandler); }
            /**
             * Sets the closure for handling whether or not to cancel a request.
             */
            inline void SetContinueRequestHandle(const ContinueRequestHandler& continueRequestHandler) { m_continueRequest = continueRequestHandler; }
            /**
             * Sets the closure for handling whether or not to cancel a request.
             */
            inline void SetContinueRequestHandle(ContinueRequestHandler&& continueRequestHandler) { m_continueRequest = std::move(continueRequestHandler); }

            /**
             * Gets the closure for receiving events when data is received from the server.
             */
            inline const DataReceivedEventHandler& GetDataReceivedEventHandler() const { return m_onDataReceived; }
            /**
             * Gets the closure for receiving events when data is sent to the server.
             */
            inline const DataSentEventHandler& GetDataSentEventHandler() const { return m_onDataSent; }

            inline const ContinueRequestHandler& GetContinueRequestHandler() const { return m_continueRequest; }

            /**
             * Gets the AWS Access Key if this HttpRequest is signed with Aws Access Key
             */
            inline const Aws::String& GetSigningAccessKey() const { return m_signingAccessKey; }
            /**
             * Sets the Aws Access Key if this HttpRequest is signed with Aws Access Key
             */
            inline void SetSigningAccessKey(const Aws::String& accessKey) { m_signingAccessKey = accessKey; }

            /**
            * Gets the signing region if this request is signed.
            */
            inline const Aws::String& GetSigningRegion() const { return m_signingRegion; }
            /**
            * Sets the signing region if this request is signed.
            */
            inline void SetSigningRegion(const Aws::String& region) { m_signingRegion = region; }

            /**
             * Add a request metric
             * @param key, HttpClientMetricsKey defined in HttpClientMetrics.cpp
             * @param value, the corresponding value of this key measured during http request.
             */
            virtual void AddRequestMetric(const Aws::String& key, int64_t value) { m_httpRequestMetrics.emplace(key, value); }

            /**
            * Sets the request metrics
            */
            virtual void SetRequestMetrics(const HttpClientMetricsCollection& collection) { m_httpRequestMetrics = collection; }

            /**
            * Gets the request metrics
            */
            virtual const HttpClientMetricsCollection& GetRequestMetrics() const { return m_httpRequestMetrics; }

            /**
             * Returns the IP address of the remote host the request was made out to.
             * This value is populated after the request is made and when the HTTP client supports retrieving such
             * information.
             * If the information is not available, an empty string is returned.
             */
            Aws::String GetResolvedRemoteHost() const { return m_resolvedRemoteHost; }
            void SetResolvedRemoteHost(const Aws::String& ip) { m_resolvedRemoteHost = ip; }

        private:
            URI m_uri;
            HttpMethod m_method;
            DataReceivedEventHandler m_onDataReceived;
            DataSentEventHandler m_onDataSent;
            ContinueRequestHandler m_continueRequest;
            Aws::String m_signingRegion;
            Aws::String m_signingAccessKey;
            Aws::String m_resolvedRemoteHost;
            HttpClientMetricsCollection m_httpRequestMetrics;
        };

    } // namespace Http
} // namespace Aws



