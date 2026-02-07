import express from 'express'
import cors from 'cors'
import 'dotenv/config'
import path from 'path'
import { fileURLToPath } from 'url'
import connectDB from './config/mongodb.js'
import connectCloudinary from './config/cloudinary.js'
import userRouter from './routes/userRoute.js'
import productRouter from './routes/productRoute.js'
import cartRouter from './routes/cartRoute.js'
import orderRouter from './routes/orderRoute.js'
import ticketRouter from './routes/ticketRoute.js'

// ES Module __dirname equivalent
const __filename = fileURLToPath(import.meta.url)
const __dirname = path.dirname(__filename)

//App config
const app = express()
const port = process.env.PORT || 4000
connectDB();
connectCloudinary();
// middlewares
app.use(express.json())
app.use(cors())

// api endpoints
app.use('/api/user', userRouter);
app.use('/api/product', productRouter);
app.use('/api/cart', cartRouter);
app.use('/api/order', orderRouter);
app.use("/uploads", express.static(path.join(__dirname, "uploads")));
app.use("/api/tickets", ticketRouter);
app.get('/', (req, res) => {
    res.send("API working")
})

app.listen(port, ()=> console.log('Server started on port: ' + port));