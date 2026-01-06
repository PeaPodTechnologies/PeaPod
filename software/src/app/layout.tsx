import type { Metadata } from 'next';
import React from 'react';
import { Geist, Geist_Mono } from 'next/font/google';
import '@/app/globals.css';
import Footer from '@/atoms/footer';
import { AppRouterCacheProvider } from '@mui/material-nextjs/v15-appRouter';
import { ThemeProvider } from '@mui/material/styles';
import theme from '../theme';

const geistSans = Geist({
  variable: '--font-geist-sans',
  subsets: ['latin'],
});

const geistMono = Geist_Mono({
  variable: '--font-geist-mono',
  subsets: ['latin'],
});

export const metadata: Metadata = {
  title: 'I2CIP.js Dashboard',
  description: 'Inter-Integrated Circuit Intranetwork Protocols',
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="en">
      <body
        className={`${geistSans.variable} ${geistMono.variable} antialiased`}
      >
        <main className="items-center justify-items-center min-h-screen p-8 pb-20">
          <AppRouterCacheProvider options={{ enableCssLayer: true }}>
            <ThemeProvider theme={theme}>{children}</ThemeProvider>
          </AppRouterCacheProvider>
        </main>
        <Footer />
      </body>
    </html>
  );
}
