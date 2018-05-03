### Terms ###
#### Log item ####
	The base data unit used, it is defined by:

	text: one or multiple text lines
	time-stamp:
	priority: optional, depending on the source

#### event record ####
	Collection of log items matching a watchers description

#### event ####
	Collection of records that are flushed on ct.
	Attachment and data fields are computed per event.

### Configuration ###
#### Root ####
        +-- instance-name
        |        type: string
        |        mandatory: true
        |        - Instance name (an instance dedicated directory is created
        |          under @root-workdir).
        +-- root-workdir
        |        type: string
        |        mandatory: true
        |        - The root working directory.
        +-- source
        |        type: object
        |        mandatory: true
        |        - The log source, check Source and Log Sources for details.
        +-- watchers
                 type: object array
                 mandatory: true
                 - The list of event watchers.

##### Source #####
        +-- type
        |       type: string
        |       mandatory: true
        |       - Identify the log source
        +-- arguments
                type: string
                mandatory: false
                - Source specific arguments

##### watcher #####
        +-- name
        |        type: string
        |        mandatory: true
        +-- start_pattern
        |        type: string
        |        mandatory: true
        |        - Reg-exp describing the start pattern of the watcher.
        +-- body_pats
        |        type: string array
        |        mandatory: false
        |        default: All the items are accepted until one of:
        |                 - @end-pattern is found
        |                 - @max-items is reached
        |                 - @flush_timeout expires
        +-- end_pattern
        |        type: string
        |        mandatory: false
        |        default: The record will be terminated when one of:
        |                 - @max-items is reached
        |                 - @flush_timeout expires
        +-- validation_pattern
        |        type: string
        |        mandatory: false
        |        - Provide a pattern that
        |          a. Is mandatory to be found within the body of
        |             the record.
        |          b. Is the pattern used for the record captures.
        +-- max_items
        |        type: integer
        |        mandatory: false
        |        default: 1
        +-- min_level
        |        type: integer
        |        mandatory: false
        |        default: 0
        |        - Minimum log level accepted by this watcher
        |          (the check is done before reg-exp checks)
        +-- max_level
        |        type: integer
        |        mandatory: false
        |        default: 7
        |        - Maximum log level accepted by this watcher
        |          (the check is done before reg-exp checks)
        +-- max_records
        |        type: integer
        |        mandatory: false
        |        default: 10
        |        - Maximum number of records in a event.
        +-- flush_timeout
        |        type: integer
        |        mandatory: false
        |        default: 120
        |        - The maximum time a record will wait before the event flush.
        +-- event_level
        |        type: integer
        |        mandatory: false
        |        default: 0
        |        - LCT event level (STAT = 0, INFO = 1, ERROR = 2)
        +-- data
        |        type: object array
        |        mandatory: false
        +-- attachments
        |        type: object array
        |        mandatory: false
        +-- max_event_count
        |        type: integer
        |        mandatory: false
        |        default: 10
        |        - The maximum number of events that should be generated in the
        |          number of seconds specified by @max_event_interval or overall
        |          (@max_event_interval = 0).
        +-- max_event_interval
        |        type: integer
        |        mandatory: false
        |        default: 60
        |        - Check @max_event_count.
        +-- event_suspend_interval
        |        type: integer
        |        mandatory: false
        |        default: 0
        |        - Defines a period (in second) in which the watcher
        |          should "ignore" new records after the generation of
        |          an event. The "ignored" records are saved and flushed
        |          out at the next "accepted" event.
        +-- max_suspend_records
                 type: integer
                 mandatory: false
                 default: 0
                 - Maximum number of records to be saved during suspend
                   (@event_suspend_interval), if any these records will be
                   dumped in suspend.txt.

###### data ######
        +-- id
        |        type: integer
        |        mandatory: true
        |        - Data field number.
        +-- repeat
        |        type: bool
        |        mandatory: false
        |        - One line added for each record, otherwise added for the
        |          first record only.
        +-- format
                 type: string
                 mandatory: true
                 - The content of the field, the following escape sequences are
                   accepted:
                    %R - total number of records in event.
                    %r - current event number.
                    %S - number of records lost during suspend
                         (check @event_suspend_interval).
                    %0 - full reg-exp capture of start/validation pattern.
                    %<1...n> - n'th capture of the start/validation pattern.

###### attachment ######
        +-- exec
        |        type: bool
        |        mandatory: false
        |        default: false
        |        - The attachment will contain the output of @src command.
        +-- src
        |        type: bool
        |        mandatory: true
        |        - Path or command.
        |          The following escape sequences are supported:
        |           %0 - full reg-exp capture of start/validation pattern of
        |             the first record.
        |           %<1...n> - n'th capture of the start/validation pattern of
        |             the first record.
        +-- dst
        |        type: string
        |         mandatory: If @exec.
        |         - The attachment destination, if not @exec and @dst is not
        |           specified, the source will not be copied into the event
        |           working dir and the source path will be provided over LCT.
        |           Same escape sequence as for @src are supported.
        +-- max_run
                 type: integer
                 mandatory: false
                 default: 10
                 - The maximum amount of time the @exec command is allowed to
                   run (seconds).

### Log Sources ###
#### kmsg ####
       The only one currently supported.
       It uses /dev/kmsg as log source.
       Supported arguments:
          "nonblock" - Process the current buffer only, do not wait for new
                       log messages.
