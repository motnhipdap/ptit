import express from 'express';
const router=express.Router();
import upload from '../middleware/upload.js';

import {createTicket, replyToTicket, getAllTickets, getTicketById, closeTicket} from '../controllers/ticketController.js'

router.post("/create",upload.single("image"),createTicket);

router.post("/:id/reply",replyToTicket)

router.get("/",getAllTickets);

router.get("/:id",getTicketById);

router.patch("/:id/close",closeTicket);


export default router;