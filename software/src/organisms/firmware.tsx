import {
  Paper,
  List,
  ListItem,
  ListItemText,
  Button,
  Typography,
} from '@mui/material';
import { FC, useState } from 'react';
import { useSocket } from '../contexts/socket';

const Firmware: FC = () => {
  const { socket } = useSocket();
  const [disable, setDisable] = useState(false);

  const flash = () => {
    if (!socket) return;
    socket.emit('firmware', {}, ({ error }: { error?: string }) => {
      if (error) {
        console.error('Firmware Flash Error:', error);
      } else {
        console.log('Firmware Flash Successful');
      }
      setDisable(false);
    });
    setDisable(true);
  };

  return (
    <Paper elevation={3} square={false} sx={{ padding: 2 }}>
      <Typography variant="h6">Firmware Flash</Typography>
      <Button
        onClick={() => {
          flash();
        }}
        disabled={!socket || socket.disconnected || disable}
      >
        Flash Firmware
      </Button>
    </Paper>
  );
};

export default Firmware;
