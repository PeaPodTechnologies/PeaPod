'use client';
import { createTheme } from '@mui/material/styles';

const theme = createTheme({
  cssVariables: true,
  palette: {
    success: {
      main: '#3db351',
    },
    error: {
      main: '#c85e41',
    },
    primary: {
      main: '#197d2a',
    },
    secondary: {
      main: '#e47862',
    },
  },
  components: {
    MuiAlert: {
      styleOverrides: {
        colorSuccess: {
          backgroundColor: '#3db351',
          color: '#ffffff',
        },
        colorError: {
          backgroundColor: '#c85e41',
          color: '#ffffff',
        },
      },
    },
  },
  typography: {
    fontFamily: 'var(--font-roboto)',
  },
});

export default theme;