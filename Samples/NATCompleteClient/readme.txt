Project: NAT Complete client

Description: All of the NAT components in one sample.

Dependencies: NATPunchthroughClient, NatTypeDetectionClient, UDPProxyClient, MiniUPnP Project
MiniUPnP project is licensed under the BSD license. See DependentExtensions\miniupnpc-1.5\LICENSE or http://miniupnp.free.fr

To build MiniUPNP
1. Include DependentExtensions\miniupnpc-1.6.20120410 in the include paths
2. Define STATICLIB in the preprocessor if necessary (See DependentExtensions\miniupnpc-1.5\declspec.h)
3. Link ws2_32.lib IPHlpApi.Lib

Related projects: ComprehensivePCGame demonstrates NATPunchthroughClient and NatTypeDetectionClient in a robust way.

For help and support, please visit http://www.jenkinssoftware.com