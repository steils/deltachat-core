project('etpan', 'c')

src = [
  'src/data-types/base64.c',
  'src/data-types/carray.c',
  'src/data-types/charconv.c',
  'src/data-types/chash.c',
  'src/data-types/clist.c',
  'src/data-types/mail_cache_db.c',
  'src/data-types/maillock.c',
  'src/data-types/mailsasl.c',
  'src/data-types/mailsem.c',
  'src/data-types/mailstream.c',
  'src/data-types/mailstream_cancel.c',
  'src/data-types/mailstream_cfstream.c',
  'src/data-types/mailstream_compress.c',
  'src/data-types/mailstream_helper.c',
  'src/data-types/mailstream_low.c',
  'src/data-types/md5.c',
  'src/data-types/mmapstring.c',
  'src/data-types/timeutils.c',
  'src/data-types/connect.c',
  'src/data-types/mailstream_socket.c',
  'src/data-types/mailstream_ssl.c',
  'src/driver/implementation/data-message/data_message_driver.c',
  'src/driver/implementation/db/dbdriver.c',
  'src/driver/implementation/db/dbdriver_message.c',
  'src/driver/implementation/db/dbstorage.c',
  'src/driver/implementation/feed/feeddriver.c',
  'src/driver/implementation/feed/feeddriver_message.c',
  'src/driver/implementation/feed/feedstorage.c',
  'src/driver/implementation/hotmail/hotmailstorage.c',
  'src/driver/implementation/imap/imapdriver.c',
  'src/driver/implementation/imap/imapdriver_cached.c',
  'src/driver/implementation/imap/imapdriver_cached_message.c',
  'src/driver/implementation/imap/imapdriver_message.c',
  'src/driver/implementation/imap/imapdriver_tools.c',
  'src/driver/implementation/imap/imapstorage.c',
  'src/driver/implementation/maildir/maildirdriver.c',
  'src/driver/implementation/maildir/maildirdriver_cached.c',
  'src/driver/implementation/maildir/maildirdriver_cached_message.c',
  'src/driver/implementation/maildir/maildirdriver_message.c',
  'src/driver/implementation/maildir/maildirdriver_tools.c',
  'src/driver/implementation/maildir/maildirstorage.c',
  'src/driver/implementation/mbox/mboxdriver.c',
  'src/driver/implementation/mbox/mboxdriver_cached.c',
  'src/driver/implementation/mbox/mboxdriver_cached_message.c',
  'src/driver/implementation/mbox/mboxdriver_message.c',
  'src/driver/implementation/mbox/mboxdriver_tools.c',
  'src/driver/implementation/mbox/mboxstorage.c',
  'src/driver/implementation/mh/mhdriver.c',
  'src/driver/implementation/mh/mhdriver_cached.c',
  'src/driver/implementation/mh/mhdriver_cached_message.c',
  'src/driver/implementation/mh/mhdriver_message.c',
  'src/driver/implementation/mh/mhdriver_tools.c',
  'src/driver/implementation/mh/mhstorage.c',
  'src/driver/implementation/mime-message/mime_message_driver.c',
  'src/driver/implementation/nntp/nntpdriver.c',
  'src/driver/implementation/nntp/nntpdriver_cached.c',
  'src/driver/implementation/nntp/nntpdriver_cached_message.c',
  'src/driver/implementation/nntp/nntpdriver_message.c',
  'src/driver/implementation/nntp/nntpdriver_tools.c',
  'src/driver/implementation/nntp/nntpstorage.c',
  'src/driver/implementation/pop3/pop3driver.c',
  'src/driver/implementation/pop3/pop3driver_cached.c',
  'src/driver/implementation/pop3/pop3driver_cached_message.c',
  'src/driver/implementation/pop3/pop3driver_message.c',
  'src/driver/implementation/pop3/pop3driver_tools.c',
  'src/driver/implementation/pop3/pop3storage.c',
  'src/driver/interface/maildriver.c',
  'src/driver/interface/maildriver_tools.c',
  'src/driver/interface/maildriver_types.c',
  'src/driver/interface/maildriver_types_helper.c',
  'src/driver/interface/mailfolder.c',
  'src/driver/interface/mailmessage.c',
  'src/driver/interface/mailmessage_tools.c',
  'src/driver/interface/mailmessage_types.c',
  'src/driver/interface/mailstorage.c',
  'src/driver/interface/mailstorage_tools.c',
  'src/engine/mailengine.c',
  'src/engine/mailprivacy.c',
  'src/engine/mailprivacy_tools.c',
  'src/low-level/imap/acl_parser.c',
  'src/low-level/imap/acl_sender.c',
  'src/low-level/imap/acl_types.c',
  'src/low-level/imap/annotatemore.c',
  'src/low-level/imap/annotatemore_parser.c',
  'src/low-level/imap/annotatemore_sender.c',
  'src/low-level/imap/annotatemore_types.c',
  'src/low-level/imap/condstore.c',
  'src/low-level/imap/condstore_types.c',
  'src/low-level/imap/enable.c',
  'src/low-level/imap/mailimap_compress.c',
  'src/low-level/imap/mailimap_helper.c',
  'src/low-level/imap/mailimap_id.c',
  'src/low-level/imap/mailimap_id_parser.c',
  'src/low-level/imap/mailimap_id_sender.c',
  'src/low-level/imap/mailimap_id_types.c',
  'src/low-level/imap/mailimap_keywords.c',
  'src/low-level/imap/mailimap_oauth2.c',
  'src/low-level/imap/mailimap_print.c',
  'src/low-level/imap/mailimap_socket.c',
  'src/low-level/imap/mailimap_sort.c',
  'src/low-level/imap/mailimap_sort_types.c',
  'src/low-level/imap/mailimap_ssl.c',
  'src/low-level/imap/mailimap_types_helper.c',
  'src/low-level/imap/namespace.c',
  'src/low-level/imap/namespace_parser.c',
  'src/low-level/imap/namespace_sender.c',
  'src/low-level/imap/namespace_types.c',
  'src/low-level/imap/qresync.c',
  'src/low-level/imap/qresync_types.c',
  'src/low-level/imap/quota.c',
  'src/low-level/imap/quota_parser.c',
  'src/low-level/imap/quota_sender.c',
  'src/low-level/imap/quota_types.c',
  'src/low-level/imap/uidplus.c',
  'src/low-level/imap/uidplus_parser.c',
  'src/low-level/imap/uidplus_sender.c',
  'src/low-level/imap/uidplus_types.c',
  'src/low-level/imap/xgmlabels.c',
  'src/low-level/imap/xgmmsgid.c',
  'src/low-level/imap/xgmthrid.c',
  'src/low-level/imap/xlist.c',
  'src/low-level/imap/acl.c',
  'src/low-level/imap/idle.c',
  'src/low-level/imap/mailimap.c',
  'src/low-level/imap/mailimap.h',
  'src/low-level/imap/mailimap_extension.c',
  'src/low-level/imap/mailimap_parser.c',
  'src/low-level/imap/mailimap_sender.c',
  'src/low-level/imap/mailimap_types.c',
  'src/low-level/imf/mailimf_types.c',
  'src/low-level/imf/mailimf_types_helper.c',
  'src/low-level/imf/mailimf_write_file.c',
  'src/low-level/imf/mailimf_write_generic.c',
  'src/low-level/imf/mailimf_write_mem.c',
  'src/low-level/imf/mailimf.c',
  'src/low-level/mime/mailmime.c',
  'src/low-level/mime/mailmime_content.c',
  'src/low-level/mime/mailmime_decode.c',
  'src/low-level/mime/mailmime_disposition.c',
  'src/low-level/mime/mailmime_types.c',
  'src/low-level/mime/mailmime_types_helper.c',
  'src/low-level/mime/mailmime_write_file.c',
  'src/low-level/mime/mailmime_write_generic.c',
  'src/low-level/mime/mailmime_write_mem.c',
  'src/low-level/smtp/mailsmtp_helper.c',
  'src/low-level/smtp/mailsmtp_oauth2.c',
  'src/low-level/smtp/mailsmtp_socket.c',
  'src/low-level/smtp/mailsmtp_ssl.c',
  'src/low-level/smtp/mailsmtp.c',
  'src/main/libetpan_version.c',
]
inc = include_directories(
  'src/data-types',
  'src/low-level/feed',
  'src/low-level/imap',
  'src/low-level/imf',
  'src/low-level/mime',
  'src/low-level/nntp',
  'src/low-level/smtp',
  'src/main',
  'src/driver/implementation/data-message',
  'src/driver/interface',
  'src/driver/tools/',
  'include',
  'include/libetpan',
)

pthreads = dependency('threads')
zlib = dependency('zlib')
openssl = dependency('openssl')
sqlite = dependency('sqlite3')
libsasl = dependency('libsasl2')
add_project_arguments('-DHAVE_CONFIG_H', language: 'c')
add_project_arguments('-DHAVE_ICONV', language: 'c')

lib = library(
  'etpan', src,
  dependencies: [pthreads, zlib, openssl, sqlite, libsasl],
  include_directories: inc,
  install: true,
)

dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: lib,
)