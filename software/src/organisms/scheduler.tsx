'use client';

import { FC, useEffect, useState } from 'react';

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
import { DebugJsonInstruction, DebugJsonMessage } from '../../api/types';
import { parseTimestamp, SchedulerEntry } from '../../api/types';
import {
  Box,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  SpeedDial,
  SpeedDialAction,
  TextField,
} from '@mui/material';
import { JsonEditor } from 'json-edit-react';
import AddIcon from '@mui/icons-material/Add';
import { Event, Restore } from '@mui/icons-material';
import { v4 as uuid } from 'uuid';
import { useStates } from '@/contexts/states';

const validateInstruction = (
  instruction: DebugJsonInstruction,
  states: Record<string, any>
): boolean => {
  if (instruction.type === 'config') {
    if (Object.keys(instruction.data).length === 0) {
      alert('Config instructions: must have data');
      return false;
    }
    if (
      !Object.keys(instruction.data).every((key) => {
        if (states[key] === undefined) {
          alert(`Config instructions: state "${key}" does not exist`);
          return false;
        }
        return true;
      })
    )
      return false;
  } else if (instruction.type === 'command') {
    if (!instruction.data) {
      alert('Command instructions: empty data');
      return false;
    }
    if (!instruction.data.fqa) {
      alert('Command instructions: no FQA provided');
      return false;
    }
    if (
      !['g', 'a', 's', 'b'].some((key) => instruction.data[key] !== undefined)
    ) {
      alert(
        'Command instructions: no valid command parameters provided (must have at least one of g, a, s, b)'
      );
      return false;
    }
  } else {
    alert('Instruction type must be either "config" or "command"');
    return false;
  }
  return true;
};

const EventModal: FC<{
  open: boolean;
  onClose: () => void;
  id: string | null;
}> = ({ open, onClose, id }) => {
  const { messages, socket } = useSocket();
  const { states } = useStates();
  const event =
    (
      messages['scheduler'] as
        | (SchedulerEntry & { entry: 'event' })[]
        | undefined
    )?.find((entry) => entry.id === id && entry.entry === 'event') ?? null;

  const [title, setTitle] = useState(event?.title ?? '');
  const [description, setDescription] = useState(event?.description ?? '');
  const [date, setDate] = useState(event?.date ?? null);
  const [instruction, setInstruction] = useState<DebugJsonInstruction>(
    event?.instruction ?? { type: 'config', data: {} }
  );

  useEffect(() => {
    setTitle(event?.title ?? '');
    setDescription(event?.description ?? '');
    setDate(event?.date ?? null);
    setInstruction(
      event?.instruction ?? {
        type: 'config',
        data: {},
      }
    );
  }, [event]);

  const resetAndClose = () => {
    setTitle('');
    setDescription('');
    setDate(null);
    setInstruction({ type: 'config', data: {} });
    onClose();
  };

  const submitAndClose = () => {
    if (!title || !date || !instruction) {
      alert('Please fill out all required fields (Title, Date, Instruction)');
      return;
    }
    if (!validateInstruction(instruction, states)) return;
    const eventData = {
      id: id ?? uuid(),
      entry: 'event',
      title,
      description,
      date,
      instruction,
    } as SchedulerEntry & { entry: 'event' };
    console.log('Submitting Event:', eventData);
    socket?.emit('scheduler-post', eventData, (error?: { error: string }) => {
      if (error) {
        alert(`Error submitting event: ${error.error}`);
      } else {
        resetAndClose();
      }
    });
  };

  return (
    <Dialog open={open} onClose={() => {}}>
      <DialogTitle>{id ? 'Edit Event' : 'New Event'}</DialogTitle>
      <DialogContent>
        <TextField
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
        </LocalizationProvider>
        <JsonEditor
          data={instruction}
          setData={(data: DebugJsonInstruction) => {
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
        />
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
  const { messages, socket } = useSocket();
  const { states } = useStates();
  const event =
    (
      messages['scheduler'] as
        | ((SchedulerEntry & { entry: 'interval' }) | undefined)[]
        | undefined
    )?.find((entry) => entry?.id === id && entry?.entry === 'interval') ?? null;

  useEffect(() => {
    setTitle(event?.title ?? '');
    setDescription(event?.description ?? '');
    setDate(event?.date ?? null);
    setInstruction(
      event?.instruction ?? {
        type: 'config',
        data: {},
      }
    );
    setInterval(event?.interval ?? 0);
  }, [event]);

  const [title, setTitle] = useState(event?.title ?? '');
  const [description, setDescription] = useState(event?.description ?? '');
  const [date, setDate] = useState(event?.date ?? null);
  const [instruction, setInstruction] = useState<DebugJsonInstruction>(
    event?.instruction ?? { type: 'config', data: {} }
  );
  const [interval, setInterval] = useState(event?.interval ?? 0);

  const resetAndClose = () => {
    setTitle('');
    setDescription('');
    setDate(null);
    setInstruction({ type: 'config', data: {} });
    setInterval(0);
    onClose();
  };

  const submitAndClose = () => {
    if (!title || !date || !instruction || !interval) {
      alert(
        'Please fill out all required fields (Title, Date, Instruction, Interval)'
      );
      return;
    }
    if (!validateInstruction(instruction, states)) return;
    const intervalData = {
      id: id ?? uuid(),
      entry: 'interval',
      title,
      description,
      date,
      instruction,
      interval,
    } as SchedulerEntry & { entry: 'interval' };
    console.log('Submitting Interval:', intervalData);
    socket?.emit(
      'scheduler-post',
      intervalData,
      (error?: { error: string }) => {
        if (error) {
          alert(`Error submitting interval: ${error.error}`);
        } else {
          resetAndClose();
        }
      }
    );
  };

  return (
    <Dialog open={open} onClose={() => {}}>
      <DialogTitle>{id ? 'Edit Interval' : 'New Interval'}</DialogTitle>
      <DialogContent>
        <TextField
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
        </LocalizationProvider>
        <TextField
          label="Interval (ms)"
          value={interval}
          onChange={(e) => setInterval(Number(e.target.value))}
          fullWidth
          type="number"
          margin="normal"
        />
        <JsonEditor
          data={instruction}
          setData={(data: DebugJsonInstruction) => {
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
        />
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
  const [date, setDate] = useState<Date | null>(null);

  const [selectedEvent, setSelectedEvent] = useState<string | null>(null);
  const [selectedInterval, setSelectedInterval] = useState<string | null>(null);
  const [eventModal, setEventModal] = useState(false);
  const [intervalModal, setIntervalModal] = useState(false);
  const [dial, setDial] = useState(false);

  const { messages, socket, startSocket, stopSocket } = useSocket();
  const { rebuild: rebuildStates } = useStates();

  const openEventModal = (id?: string) => {
    rebuildStates();
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
    rebuildStates();
    setTimeout(stopSocket, 1000);
    setSelectedInterval(id ?? null);
    setIntervalModal(true);
  };

  const closeIntervalModal = () => {
    setSelectedInterval(null);
    setIntervalModal(false);
    startSocket();
  };

  const resetSchedulerMessages = () => (messages['scheduler'] = []);

  const deleteEvent = (id: string) => {
    if (!window.confirm('Are you sure you want to delete this entry?')) return;
    resetSchedulerMessages(); // Temp fix
    socket.emit('scheduler-delete', id, (error?: { error: string }) => {
      if (error) {
        alert(`Error deleting entry: ${error.error}`);
      }
    });
  };

  const deduped = Array.from(
    new Map(messages['scheduler']?.map((item) => [item.id, item])).values()
  );

  const todayMessages: SchedulerEntry[] = date
    ? deduped?.filter((entry) => dayjs(entry.date).isSame(dayjs(date), 'day'))
    : deduped || [];

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
                  openEventModal(params.row.id);
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
                  openIntervalModal(params.row.id);
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
                  deleteEvent(params.row.id);
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
      headerName: 'Title',
      width: 150,
    },
    {
      field: 'description',
      headerName: 'Description',
      width: 150,
    },
    {
      field: 'date',
      headerName: 'Date',
      width: 600,
      valueGetter: (value, row) => {
        switch (row.entry) {
          case 'event':
            return `${new Date(row.date)?.toDateString()} ${new Date(row.date)?.toLocaleTimeString()}`;
          case 'interval':
            return `Every ${parseTimestamp(row.interval)} Starting ${new Date(row.date)?.toDateString()} ${new Date(row.date)?.toLocaleTimeString()} Until ${new Date(row.endDate)?.toDateString()} ${new Date(row.endDate)?.toLocaleTimeString()}`;
        }
      },
    },
  ];

  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Scheduler</Typography>
      <LocalizationProvider dateAdapter={AdapterDayjs}>
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
      </LocalizationProvider>
      <DataGrid
        rows={todayMessages}
        columns={DATAGRID_COLUMNS}
        autoHeight
        getRowId={(row) => row.id}
      />
      <EventModal
        open={eventModal}
        onClose={closeEventModal}
        id={selectedEvent}
      />
      <Box sx={{ display: 'flex', justifyContent: 'flex-end', gap: 2, mt: 2 }}>
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
