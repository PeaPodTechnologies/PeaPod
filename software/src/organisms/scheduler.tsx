'use client';

import { FC, useEffect, useMemo, useState } from 'react';

import dayjs from 'dayjs';
import Paper from '@mui/material/Paper';
import Typography from '@mui/material/Typography';
import Badge from '@mui/material/Badge';
import { AdapterDayjs } from '@mui/x-date-pickers/AdapterDayjs';
import { DateCalendar } from '@mui/x-date-pickers/DateCalendar';
import { LocalizationProvider } from '@mui/x-date-pickers/LocalizationProvider';
import { DateTimePicker } from '@mui/x-date-pickers/DateTimePicker';
import { PickerDay } from '@mui/x-date-pickers/PickerDay';
import { useSocket } from '../contexts/socket';
import Button from '@mui/material/Button';
import { DataGrid } from '@mui/x-data-grid/DataGrid';
import { GridColDef } from '@mui/x-data-grid';
import { DebugJsonInstruction } from '../../api/types';
import { parseTimestamp } from '../../api/types';
import {
  Box,
  Checkbox,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  FormControl,
  InputLabel,
  MenuItem,
  Select,
  SpeedDial,
  SpeedDialAction,
  TextField,
} from '@mui/material';
import { JsonEditor } from 'json-edit-react';
import AddIcon from '@mui/icons-material/Add';
import { Event, Restore } from '@mui/icons-material';
import { v4 as uuid } from 'uuid';
import { useStates } from '@/contexts/states';
import { EventTree, useEvents } from '@/contexts/events';
import { IntervalTree, useIntervals } from '@/contexts/intervals';

// const validateInstruction = (
//   instruction: DebugJsonInstruction,
//   states: Record<string, any>
// ): boolean => {
//   if (instruction.type === 'config') {
//     if (Object.keys(instruction.data).length === 0) {
//       alert('Config instructions: must have data');
//       return false;
//     }
//     if (
//       !Object.keys(instruction.data).every((key) => {
//         if (states[key] === undefined) {
//           alert(`Config instructions: state "${key}" does not exist`);
//           return false;
//         }
//         return true;
//       })
//     )
//       return false;
//   } else if (instruction.type === 'command') {
//     if (!instruction.data) {
//       alert('Command instructions: empty data');
//       return false;
//     }
//     if (!instruction.data.fqa) {
//       alert('Command instructions: no FQA provided');
//       return false;
//     }
//     if (
//       !['g', 'a', 's', 'b'].some((key) => instruction.data[key] !== undefined)
//     ) {
//       alert(
//         'Command instructions: no valid command parameters provided (must have at least one of g, a, s, b)'
//       );
//       return false;
//     }
//   } else {
//     alert('Instruction type must be either "config" or "command"');
//     return false;
//   }
//   return true;
// };

const EventModal: FC<{
  open: boolean;
  onClose: () => void;
  id: string | null;
}> = ({ open, onClose, id }) => {
  const { socket, latestTimestamp } = useSocket();
  const { flags } = useStates();
  const { events } = useEvents();
  const event = id ? (events[id] ?? null) : null;

  //   (
  //     messages['scheduler'] as
  //       | (SchedulerEntry & { entry: 'event' })[]
  //       | undefined
  //   )?.find((entry) => entry.id === id && entry.entry === 'event') ?? null;

  // const [title, setTitle] = useState(event?.key ?? '');
  // const [description, setDescription] = useState(event?.description ?? '');
  // const [date, setDate] = useState(event?.date ?? null);
  const [delta, setDelta] = useState(event?.delta ?? latestTimestamp);
  // const [instruction, setInstruction] = useState<DebugJsonInstruction>(
  //   event?.instruction ?? { type: 'config', data: {} }
  // );
  const [flag, setFlag] = useState(event?.flag ?? '');
  const [invert, setInvert] = useState(event?.invert ?? false);

  useEffect(() => {
    // setTitle(event?.key ?? '');
    setDelta(event?.delta ?? latestTimestamp);
    setFlag(event?.flag ?? '');
    setInvert(event?.invert ?? false);
  }, [event, latestTimestamp]);

  const resetAndClose = () => {
    // setTitle('');
    // setDescription('');
    // setDate(null);
    // setInstruction({ type: 'config', data: {} });
    setDelta(0);
    setFlag('');
    setInvert(false);
    onClose();
  };

  const submitAndClose = () => {
    if (!delta || !flag) {
      alert('Please fill out all required fields (Delta, Flag)');
      return;
    }
    // if (!validateInstruction(instruction, states)) return;
    const eventData = {
      type: 'event',
      // title,
      // description,
      // date,
      // instruction,
      data: { delta, flag, invert },
    };
    console.log('Submitting Event:', eventData);
    socket?.emit('serialinput', eventData, (error?: { error: string }) => {
      if (error) {
        alert(`Error submitting event: ${error.error}`);
      } else {
        resetAndClose();
      }
    });
  };

  return (
    <Dialog open={open} onClose={() => {}}>
      <DialogTitle>Event</DialogTitle>
      <DialogContent>
        {/* <TextField
          label="Title"
          value={title}
          onChange={(e) => setTitle(e.target.value)}
          fullWidth
          margin="normal"
        /> */}
        {/* <TextField
          label="Description"
          value={description}
          onChange={(e) => setDescription(e.target.value)}
          fullWidth
          margin="normal"
        /> */}
        {/* <LocalizationProvider dateAdapter={AdapterDayjs}>
          <DateTimePicker
            views={['year', 'month', 'day', 'hours', 'minutes', 'seconds']}
            sx={{ mt: 2 }}
            label="Date & Time"
            value={dayjs(date)}
            onChange={(newValue) => {
              setDate(newValue?.toDate() ?? new Date());
            }}
          />
        </LocalizationProvider> */}
        <TextField
          label="Timestamp"
          type="number"
          value={delta}
          onChange={(e) => setDelta(Number(e.target.value))}
          fullWidth
          margin="normal"
        />
        {/* <JsonEditor
          data={instruction}
          // eslint-disable-next-line @typescript-eslint/no-explicit-any
          setData={(data: any) => {
            if (!data.type) {
              alert('Instruction must have a type');
              return false;
            }
            if (data.type === 'config') {
              if (Object.keys(data.data).length === 0) {
                alert('Config instructions: must have data');
                return;
              }
              if (
                !Object.keys(data.data).every((key) => {
                  if (states[key] === undefined) {
                    alert(`Config instructions: state "${key}" does not exist`);
                    return false;
                  }
                  return true;
                })
              )
                return;
            }
            console.log('Setting Instruction:', data);
            setInstruction(data as DebugJsonInstruction);
          }}
        /> */}
        <FormControl fullWidth>
          <InputLabel id="flag-label">Flag</InputLabel>
          <Select
            label="Flag"
            value={flag}
            onChange={(e) => setFlag(e.target.value)}
          >
            {Object.keys(flags).map((key) => (
              <MenuItem key={key} value={key}>
                {key}
              </MenuItem>
            ))}
          </Select>
        </FormControl>
        <FormControl fullWidth>
          <InputLabel id="flag-label">Invert?</InputLabel>
          <Checkbox
            checked={invert}
            onChange={(e) => setInvert(e.target.checked)}
          />
        </FormControl>
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={resetAndClose} autoFocus>
          Cancel
        </Button>
        <Button variant="contained" onClick={submitAndClose}>
          Submit
        </Button>
      </DialogActions>
    </Dialog>
  );
};

const IntervalModal: FC<{
  open: boolean;
  onClose: () => void;
  id: string | null;
}> = ({ open, onClose, id }) => {
  const { socket } = useSocket();
  const { flags } = useStates();
  const { intervals } = useIntervals();
  const interval = id ? (intervals[id] ?? null) : null;
  //   (
  //     messages['scheduler'] as
  //       | (SchedulerEntry & { entry: 'event' })[]
  //       | undefined
  //   )?.find((entry) => entry.id === id && entry.entry === 'event') ?? null;

  // const [title, setTitle] = useState(event?.key ?? '');
  // const [description, setDescription] = useState(event?.description ?? '');
  // const [date, setDate] = useState(event?.date ?? null);
  const [delta, setDelta] = useState(interval?.delta ?? 0);
  const [phase, setPhase] = useState(interval?.phase ?? 0);
  // const [instruction, setInstruction] = useState<DebugJsonInstruction>(
  //   event?.instruction ?? { type: 'config', data: {} }
  // );
  const [flag, setFlag] = useState(interval?.flag ?? '');
  const [invert, setInvert] = useState(interval?.invert ?? false);

  useEffect(() => {
    // setTitle(event?.key ?? '');
    setDelta(interval?.delta ?? 0);
    setPhase(interval?.phase ?? 0);
    setFlag(interval?.flag ?? '');
    setInvert(interval?.invert ?? false);
  }, [interval]);

  const resetAndClose = () => {
    // setTitle('');
    // setDescription('');
    // setDate(null);
    // setInstruction({ type: 'config', data: {} });
    setDelta(0);
    setPhase(0);
    setFlag('');
    setInvert(false);
    onClose();
  };

  const submitAndClose = () => {
    if (!delta || !flag) {
      alert('Please fill out all required fields (Delta, Flag)');
      return;
    }
    // if (!validateInstruction(instruction, states)) return;
    const eventData = {
      type: 'interval',
      // title,
      // description,
      // date,
      // instruction,
      data: { delta, phase, flag, invert },
    };
    console.log('Submitting Interval:', eventData);
    socket?.emit('serialinput', eventData, (error?: { error: string }) => {
      if (error) {
        alert(`Error submitting event: ${error.error}`);
      } else {
        resetAndClose();
      }
    });
  };

  return (
    <Dialog open={open} onClose={() => {}}>
      <DialogTitle>Interval</DialogTitle>
      <DialogContent>
        {/* <TextField
          label="Title"
          value={title}
          onChange={(e) => setTitle(e.target.value)}
          fullWidth
          margin="normal"
        />
        <TextField
          label="Description"
          value={description}
          onChange={(e) => setDescription(e.target.value)}
          fullWidth
          margin="normal"
        />
        <LocalizationProvider dateAdapter={AdapterDayjs}>
          <DateTimePicker
            views={['year', 'month', 'day', 'hours', 'minutes', 'seconds']}
            sx={{ mt: 2 }}
            label="Date & Time"
            value={dayjs(date)}
            onChange={(newValue) => {
              setDate(newValue?.toDate() ?? new Date());
            }}
          />
        </LocalizationProvider> */}
        <TextField
          label="Interval (ms)"
          value={delta}
          onChange={(e) => setDelta(Number(e.target.value))}
          fullWidth
          type="number"
          margin="normal"
        />
        <TextField
          label="Phase (ms)"
          value={phase}
          onChange={(e) => setPhase(Number(e.target.value))}
          fullWidth
          type="number"
          margin="normal"
        />
        {/* <JsonEditor
          data={instruction}
          // eslint-disable-next-line @typescript-eslint/no-explicit-any
          setData={(data: any) => {
            if (!data.type) {
              alert('Instruction must have a type');
              return false;
            }
            if (data.type === 'config') {
              if (Object.keys(data.data).length === 0) {
                alert('Config instructions: must have data');
                return;
              }
              if (
                !Object.keys(data.data).every((key) => {
                  if (states[key] === undefined) {
                    alert(`Config instructions: state "${key}" does not exist`);
                    return false;
                  }
                  return true;
                })
              )
                return;
            }
            console.log('Setting Instruction:', data);
            setInstruction(data as DebugJsonInstruction);
          }}
        /> */}
        <FormControl fullWidth>
          <InputLabel id="flag-label">Flag</InputLabel>
          <Select
            label="Flag"
            value={flag}
            onChange={(e) => setFlag(e.target.value)}
          >
            {Object.keys(flags).map((key) => (
              <MenuItem key={key} value={key}>
                {key}
              </MenuItem>
            ))}
          </Select>
        </FormControl>
        <FormControl fullWidth>
          <InputLabel id="flag-label">Invert?</InputLabel>
          <Checkbox
            checked={invert}
            onChange={(e) => setInvert(e.target.checked)}
          />
        </FormControl>
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={resetAndClose} autoFocus>
          Cancel
        </Button>
        <Button variant="contained" onClick={submitAndClose}>
          Submit
        </Button>
      </DialogActions>
    </Dialog>
  );
};

const Scheduler: FC = () => {
  // const [date, setDate] = useState<Date | null>(null);

  const [selectedEvent, setSelectedEvent] = useState<string | null>(null);
  const [selectedInterval, setSelectedInterval] = useState<string | null>(null);
  const [eventModal, setEventModal] = useState(false);
  const [intervalModal, setIntervalModal] = useState(false);
  const [dial, setDial] = useState(false);

  const { socket, startSocket, stopSocket } = useSocket();
  const { rebuild: rebuildStates } = useStates();
  const { events, rebuild: rebuildEvents } = useEvents();
  const { intervals, rebuild: rebuildIntervals } = useIntervals();

  const openEventModal = (id?: string) => {
    rebuildStates((response) => {
      if (response && response.error) {
        alert(`State rebuild failed: ${response.error}`);
      }
    });
    rebuildEvents((response) => {
      if (response && response.error) {
        alert(`Event rebuild failed: ${response.error}`);
      }
    });
    setTimeout(stopSocket, 1000);
    setSelectedEvent(id ?? null);
    setEventModal(true);
  };

  const closeEventModal = () => {
    setSelectedEvent(null);
    setEventModal(false);
    startSocket();
  };

  const openIntervalModal = (id?: string) => {
    rebuildStates((response) => {
      if (response && response.error) {
        alert(`State rebuild failed: ${response.error}`);
      }
    });
    rebuildIntervals((response) => {
      if (response && response.error) {
        alert(`Interval rebuild failed: ${response.error}`);
      }
    });
    setTimeout(stopSocket, 1000);
    setSelectedInterval(id ?? null);
    setIntervalModal(true);
  };

  const closeIntervalModal = () => {
    setSelectedInterval(null);
    setIntervalModal(false);
    startSocket();
  };

  // const resetSchedulerMessages = () => (messages['scheduler'] = []);

  const deleteEvent = (id: string) => {
    if (!window.confirm('Are you sure you want to delete this entry?')) return;
    // resetSchedulerMessages(); // Temp fix
    if (!socket) {
      alert('No socket connection');
      return;
    }
    socket.emit('scheduler-delete', id, (error?: { error: string }) => {
      if (error) {
        alert(`Error deleting entry: ${error.error}`);
      }
    });
  };

  // const deduped = Array.from(
  //   new Map(messages['scheduler']?.map((item) => [item.id, item])).values()
  // );

  // const todayMessages: SchedulerEntry[] = date
  //   ? deduped?.filter((entry) => dayjs(entry.date).isSame(dayjs(date), 'day'))
  //   : deduped || [];

  type SchedulerEntry =
    | (EventTree[string] & { entry: 'event' })
    | (IntervalTree[string] & { entry: 'interval' });

  const entries: SchedulerEntry[] = useMemo(() => {
    const eventEntries = Object.entries(events).map<SchedulerEntry>(
      ([id, event]) => ({
        ...event,
        entry: 'event',
      })
    );
    const intervalEntries = Object.entries(intervals).map<SchedulerEntry>(
      ([id, interval]) => ({
        ...interval,
        entry: 'interval',
      })
    );
    return [...eventEntries, ...intervalEntries].sort((a, b) => {
      return a.timestamp - b.timestamp;
    });
  }, [events, intervals]);

  const DATAGRID_COLUMNS: GridColDef<SchedulerEntry>[] = [
    {
      field: 'edit',
      headerName: '',
      width: 100,
      renderCell: (params) => {
        switch (params.row.entry) {
          case 'event':
            return (
              <Button
                variant="outlined"
                size="small"
                onClick={() => {
                  openEventModal(params.row.key);
                }}
              >
                View
              </Button>
            );
          case 'interval':
            return (
              <Button
                variant="outlined"
                size="small"
                onClick={() => {
                  openIntervalModal(params.row.key);
                }}
              >
                View
              </Button>
            );
          default:
            return null;
        }
      },
    },
    {
      field: 'delete',
      headerName: '',
      width: 100,
      renderCell: (params) => {
        switch (params.row.entry) {
          case 'event':
          case 'interval':
            return (
              <Button
                variant="outlined"
                size="small"
                onClick={() => {
                  deleteEvent(params.row.key);
                }}
              >
                Delete
              </Button>
            );
          default:
            return null;
        }
      },
    },
    {
      field: 'type',
      headerName: 'Type',
      width: 160,

      valueGetter: (value, row) => {
        switch (row.entry) {
          case 'event':
            return 'Event';
          case 'interval':
            return 'Interval';
        }
      },
    },
    {
      field: 'title',
      headerName: 'Key',
      width: 150,
      valueGetter: (value, row) => {
        switch (row.entry) {
          case 'event':
            return row.key;
          case 'interval':
            return row.key;
        }
      },
    },
    {
      field: 'flag',
      headerName: 'Flag',
      width: 150,
      valueGetter: (value, row) => {
        switch (row.entry) {
          case 'event':
            return row.flag || 'N/A';
          case 'interval':
            return row.flag || 'N/A';
        }
      },
    },
    {
      field: 'date',
      headerName: 'Timestamp',
      width: 600,
      valueGetter: (value, row) => {
        switch (row.entry) {
          case 'event':
            return `${parseTimestamp(row.delta)} Since Program Start`;
          case 'interval':
            return `Delta: ${parseTimestamp(row.delta)}, Phase: ${parseTimestamp(row.phase)} Since Program Start`;
        }
      },
    },
  ];

  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Scheduler</Typography>
      {/* <LocalizationProvider dateAdapter={AdapterDayjs}>
        <DateCalendar
          value={date ? dayjs(date) : null}
          onChange={(newValue) => setDate(newValue ? newValue.toDate() : null)}
          yearsOrder="desc"
          showDaysOutsideCurrentMonth
          slots={{
            day: (dayProps) => {
              const isSelected =
                dayProps.day.isSame(date, 'day') && date !== null;
              const hasActivity = deduped?.some((entry) =>
                dayProps.day.isSame(dayjs(entry.date), 'day')
              );
              return (
                <Badge
                  variant="dot"
                  invisible={!hasActivity || isSelected}
                  color="secondary"
                  overlap="circular"
                  anchorOrigin={{
                    vertical: 'bottom',
                    horizontal: 'left',
                  }}
                  sx={{
                    '& .MuiBadge-badge': {
                      left: '50%',
                      transform: 'translateX(-50%)',
                      bottom: 4,
                    },
                  }}
                >
                  <PickerDay {...dayProps} />
                </Badge>
              );
            },
          }}
        />
        <Button
          variant="contained"
          sx={{ mb: 2 }}
          disabled={!date}
          onClick={() => setDate(null)}
        >
          Show All
        </Button>
        <Button
          variant="contained"
          sx={{ mb: 2, ml: 2 }}
          onClick={() => setDate(new Date())}
          disabled={date ? dayjs().isSame(date, 'day') : false}
        >
          Show Today
        </Button>
      </LocalizationProvider> */}
      <DataGrid
        // rows={todayMessages}
        rows={entries || []}
        columns={DATAGRID_COLUMNS}
        autoHeight
        getRowId={(row) => row.key}
      />
      <EventModal
        open={eventModal}
        onClose={closeEventModal}
        id={selectedEvent}
      />
      <Box sx={{ display: 'flex', justifyContent: 'flex-end', gap: 2, mt: 2 }}>
        <Button
          variant="outlined"
          onClick={() => {
            rebuildStates((response) => {
              if (response && response.error) {
                alert(`State rebuild failed: ${response.error}`);
              }
            });
            rebuildEvents((response) => {
              if (response && response.error) {
                alert(`Event rebuild failed: ${response.error}`);
              }
            });
            rebuildIntervals((response) => {
              if (response && response.error) {
                alert(`Interval rebuild failed: ${response.error}`);
              }
            });
          }}
        >
          Rebuild
        </Button>
        <Button
          variant="contained"
          onClick={() =>
            socket &&
            socket.emit('scheduler-save', (error: { error: string }) => {
              if (error) {
                alert(`Error saving scheduler: ${error.error}`);
              } else {
                alert('Scheduler saved successfully!');
              }
            })
          }
        >
          Save to Disk
        </Button>
        <Button
          variant="contained"
          color="secondary"
          onClick={() =>
            socket &&
            socket.emit('scheduler-load', (error: { error: string }) => {
              if (error) {
                alert(`Error loading scheduler: ${error.error}`);
              } else {
                alert('Scheduler loaded successfully!');
              }
            })
          }
        >
          Load & Overwrite
        </Button>
      </Box>
      <IntervalModal
        open={intervalModal}
        onClose={closeIntervalModal}
        id={selectedInterval}
      />
      <SpeedDial
        ariaLabel="speed-dial"
        sx={{
          position: 'fixed',
          bottom: 32,
          right: 32,
          zIndex: (theme) => theme.zIndex.speedDial,
          overflow: 'visible',
        }}
        icon={<AddIcon />}
        FabProps={{
          sx: {
            width: '4.5rem',
            height: '4.5rem',
          },
        }}
        open={dial}
        onClick={() => {
          setDial((s) => !s);
        }}
      >
        <SpeedDialAction
          icon={<Event />}
          sx={{
            '& .MuiSpeedDialAction-staticTooltipLabel': {
              whiteSpace: 'nowrap',
              maxWidth: 'none',
              flexShrink: 0,
            },
          }}
          slotProps={{
            fab: {
              size: 'large',
            },
            tooltip: {
              title: 'Add Event',
              open: true,
            },
          }}
          onClick={() => openEventModal()}
        />
        <SpeedDialAction
          icon={<Restore />}
          sx={{
            '& .MuiSpeedDialAction-staticTooltipLabel': {
              whiteSpace: 'nowrap',
              maxWidth: 'none',
              flexShrink: 0,
            },
          }}
          slotProps={{
            tooltip: {
              title: 'Add Interval',
              open: true,
            },
            fab: {
              size: 'large',
            },
          }}
          onClick={() => openIntervalModal()}
        />
      </SpeedDial>
    </Paper>
  );
};

export default Scheduler;
