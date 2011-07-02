#ifndef __INTROSPECTION_XML__
#define __INTROSPECTION_XML__
/*
 * MPRIS v1
 */
const gchar xml_v1_root[] =
"<node>"
"<interface name='org.freedesktop.MediaPlayer'>"
"    <method name='Identity'>"
"        <arg type='s' direction='out'/>"
"    </method>"
"    <method name='Quit'>"
"    </method>"
"    <method name='MprisVersion'>"
"        <arg type='(qq)' direction='out'/>"
"    </method>"
"</interface>"
"</node>";

const gchar xml_v1_tracklist[] =
"<node>"
"<interface name='org.freedesktop.MediaPlayer'>"
"    <method name='GetMetadata'>"
"        <arg type='i' direction='in' />"
"        <arg type='a{sv}' direction='out' />"
"    </method>"
"    <method name='GetCurrentTrack'>"
"        <arg type='i' direction='out' />"
"    </method>"
"    <method name='GetLength'>"
"        <arg type='i' direction='out' />"
"    </method>"
"    <method name='AddTrack'>"
"        <arg type='s' direction='in' />"
"        <arg type='b' direction='in' />"
"        <arg type='i' direction='out' />"
"    </method>"
"    <method name='DelTrack'>"
"        <arg type='i' />"
"    </method>"
"    <method name='SetLoop'>"
"        <arg type='b' />"
"    </method>"
"    <method name='SetRandom'>"
"        <arg type='b' />"
"    </method>"
"    <signal name='TrackListChange'>"
"        <arg type='i' />"
"    </signal>  "
"</interface>"
"</node>";

const gchar xml_v1_player[] =
"<node>"
"<interface name='org.freedesktop.MediaPlayer'>"
"    <method name='Next'>"
"    </method>"
"    <method name='Prev'>"
"    </method>"
"    <method name='Pause'>"
"    </method>"
"    <method name='Stop'>"
"    </method>"
"    <method name='Play'>"
"    </method>"
"    <method name='Repeat'>"
"        <arg type='b' direction='in'/>"
"    </method>"
"    <method name='GetStatus'>"
"        <arg type='(iiii)' direction='out'/>"
"    </method>"
"    <method name='GetMetadata'>"
"        <arg type='a{sv}' direction='out' />"
"    </method>"
"    <method name='GetCaps'>"
"        <arg type='i' direction='out' />"
"    </method>"
"    <signal name='CapsChange'>"
"        <arg type='i' />"
"    </signal>"
"    <method name='VolumeSet'>"
"        <arg type='i'/>"
"    </method>"
"    <method name='VolumeGet'>"
"        <arg type='i' direction='out'/>"
"    </method>"
"    <method name='PositionSet'>"
"        <arg type='i'/>"
"    </method>"
"    <method name='PositionGet'>"
"        <arg type='i' direction='out'/>"
"    </method>"
"    <signal name='TrackChange'>"
"        <arg type='a{sv}'/>"
"    </signal>"
"    <signal name='StatusChange'>"
"        <arg type='(iiii)'/>"
"    </signal>"
"    <signal name='CapsChange'>"
"        <arg type='i' />"
"    </signal>"
"</interface>"
"</node>";

/*
 * MPRIS V2
 */
const gchar xml_v2_root[] =
"<node>"
"<interface name='org.mpris.MediaPlayer2'>"
"    <method name='Raise'>"
"    </method>"
"    <method name='Quit'>"
"    </method>"

"    <property type='b' name='CanQuit' access='read'/>
"    <property type='b' name='CanRaise' access='read'/>
"    <property type='b' name='HasTrackList' access='read'/>
"    <property type='s' name='Identity' access='read'/>
"    <property type='s' name='DesktopEntry' access='read'/>
"    <property type='as' name='SupportedUriSchemes' access='read'/>
"    <property type='as' name='SupportedMimeTypes' access='read'/>
"</interface>"
"</node>";

const gchar xml_v2_tracklist[] =
"<node>"
"<interface name='org.mpris.MediaPlayer2.TrackList'>"
"    <method name='GetTracksMetadata'>"
"        <arg type='ao' name='trackIds' direction='in' />"
"        <arg type='aa{sv}' direction='out' />"
"    </method>"
"    <method name='AddTrack'>"
"        <arg type='s' name='uri' direction='in'/>"
"        <arg type='o' name='afterTrack' direction=in'/>"
"        <arg type='b' name='setAsCurrent' direction='in'/>"
"    </method>"
"    <method name='RemoveTrack'>"
"        <arg type='o' name='trackId' direction=in'/>"
"    </method>"
"    <method name='GoTo'>"
"        <arg type='o' name='trackId' direction=in'/>"
"    </method>"

"    <signal name='TrackListReplaced'>"
"        <arg type='ao' name='tracks'/>"
"        <arg type='o' name='currentTrack'/>"
"    </signal>  "
"    <signal name='TrackAdded'>"
"        <arg type='a{sv}' name='metadata'/>"
"        <arg type='o' name='afterTrack'/>"
"    </signal>  "
"    <signal name='TrackRemoved'>"
"        <arg type='o' name='trackId'/>"
"    </signal>  "
"    <signal name='TrackMetadataChanged'>"
"        <arg type='a{sv}' name='metadata'/>"
"        <arg type='o' name='trackId'/>"
"    </signal>  "

"    <property type='ao' name='Tracks' access='read'/>
"    <property type='b' name='CanEditTracks' access='read'/>
"</interface>"
"</node>";

const gchar xml_v2_player[] =
"<node>"
"<interface name='org.mpris.MediaPlayer2.Player'>"
"    <method name='Next'>"
"    </method>"
"    <method name='Previous'>"
"    </method>"
"    <method name='Pause'>"
"    </method>"
"    <method name='PlayPause'>"
"    </method>"
"    <method name='Stop'>"
"    </method>"
"    <method name='Play'>"
"    </method>"
"    <method name='Seek'>"
"        <arg type='x' name='offset' direction='in'/>"
"    </method>"
"    <method name='SetPosition'>"
"        <arg type='o' name='trackId' direction='in'/>"
"        <arg type='x' name='position' direction='in'/>"
"    </method>"
"    <method name='OpenUri'>"
"        <arg type='s' name='uri' direction='in'/>"
"    </method>"

"    <signal name='Seeked'>"
"        <arg type='x' name='position'/>"
"    </signal>  "

"    <property type='s' name='PlaybackStatus' access='read'/>
"    <property type='s' name='LoopStatus' access='readwrite'/>
"    <property type='d' name='Rate' access='readwrite'/>
"    <property type='b' name='Shuffle' access='readwrite'/>
"    <property type='a{sv}' name='Metadata' access='read'/>
"    <property type='d' name='Volume' access='readwrite'/>
"    <property type='x' name='Position' access='read'/>
"    <property type='d' name='MinimumRate' access='read'/>
"    <property type='d' name='MaximumRate' access='read'/>
"    <property type='b' name='CanGoNext' access='read'/>
"    <property type='b' name='CanGoPrevious' access='read'/>
"    <property type='b' name='CanPlay' access='read'/>
"    <property type='b' name='CanPause' access='read'/>
"    <property type='b' name='CanSeek' access='read'/>
"    <property type='b' name='CanControl' access='read'/>
"</interface>"
"</node>";

const gchar xml_v2_playerlist[] =
"<node>"
"<interface name='org.mpris.MediaPlayer2.PlayerList'>"
"    <method name='ActivatePlaylist'>"
"        <arg type='o' name='playlistId' direction='in' />"
"    </method>"
"    <method name='GetPlaylists'>"
"        <arg type='u' name='index' direction='in'/>"
"        <arg type='u' name='maxCount' direction='in'/>"
"        <arg type='s' name='order' direction='in'/>"
"        <arg type='b' name='reverseOrder' direction=in'/>"
"        <arg type='a(oss)' name='playlists' direction='out'/>"
"    </method>"

"    <signal name='PlaylistChanged'>"
"        <arg type='(oss)' name='playlist'/>"
"    </signal>  "

"    <property type='u' name='PlaylistCount' access='read'/>
"    <property type='as' name='Orderings' access='read'/>
"    <property type='b(oss)' name='ActivePlaylist' access='read'/>
"</interface>"
#endif
