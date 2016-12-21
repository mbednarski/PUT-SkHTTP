using System.Collections;
using System.Collections.Generic;

namespace SkHttpClient.Model
{
    public static class RequestFactory
    {
        public static SkRequest CreateGet(string resource)
        {
            return new SkRequest(HttpMethod.GET, resource);
        }

        public static SkRequest CreatePut(string resource, byte[] payload)
        {
            return new SkRequest(HttpMethod.PUT, resource, payload);
        }

        public static SkRequest CreateHead(string resource)
        {
            return new SkRequest(HttpMethod.HEAD, resource);
        }

        public static SkRequest CreateDelete(string resource)
        {
            return new SkRequest(HttpMethod.DELETE, resource);
        }
    }
}