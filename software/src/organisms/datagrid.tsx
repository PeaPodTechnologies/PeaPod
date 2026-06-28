import { Paper, Typography } from '@mui/material';
import {
  GridColDef,
  GridRowsProp,
  DataGrid,
  GridColumnResizeParams,
} from '@mui/x-data-grid';
import { FC, useState } from 'react';
import { useSocket } from '../contexts/socket';

const DEBUGJSON_NUM_DATA_OPTIONS = [5, 10, 25, 50];

type MessageDataGridProps = {
  socket: string;
};

const MessageDataGrid: FC<MessageDataGridProps> = ({ socket }) => {
  const { messages } = useSocket();
  const [columnWidths, setColumnWidths] = useState({
    timestamp: 180,
    type: 150,
    data: 300,
    msg: 300,
  });

  const columns: GridColDef[] = [
    {
      field: 'timestamp',
      headerName: 'Timestamp',
      width: columnWidths.timestamp,
    },
    { field: 'type', headerName: 'Type', width: columnWidths.type },
    { field: 'data', headerName: 'Data', width: columnWidths.data },
    { field: 'msg', headerName: 'Message', width: columnWidths.msg },
  ];

  const rows: GridRowsProp = messages[socket]
    ?.filter((message) => message['type'] !== 'heartbeat')
    .map((message, id) => ({
      id,
      timestamp: message['timestamp'],
      type: message['type'],
      msg: message['msg'],
      data: JSON.stringify({
        ...message,
        timestamp: undefined,
        type: undefined,
        msg: undefined,
      }),
    }));

  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6" gutterBottom>
        I2CIP Messages for Socket: {socket}
      </Typography>
      <DataGrid
        rows={rows}
        columns={columns}
        initialState={{
          pagination: {
            paginationModel: {
              pageSize: DEBUGJSON_NUM_DATA_OPTIONS[0],
            },
          },
        }}
        pageSizeOptions={DEBUGJSON_NUM_DATA_OPTIONS}
        checkboxSelection
        disableRowSelectionOnClick
        onColumnWidthChange={(params: GridColumnResizeParams) => {
          setColumnWidths((prev) => ({
            ...prev,
            [params.colDef.field]: params.width,
          }));
        }}
      />
    </Paper>
  );
};

export default MessageDataGrid;
