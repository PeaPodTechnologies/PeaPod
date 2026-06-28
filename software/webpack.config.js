// eslint-disable-next-line import/no-anonymous-default-export
export default {
  // other webpack config
  module: {
    rules: [
      {
        test: /\.css$/i,
        use: ['style-loader', 'css-loader'],
      },
    ],
  },
};